#include "messagehandler.h"
#include <QFileInfo>

MessageHandler::MessageHandler( IRCClient *irc, QWidget *parent ) : QTabWidget( parent ) {
	this->scriptManager = new MIRCScriptManager();
	connect(this->scriptManager, SIGNAL( unknown_alias( QString, QStringList ) ), this, SLOT( call_alias( QString, QStringList ) ) );
	this->irc = irc;
	connect( irc, SIGNAL( messageRcvd( QString, QString, QString, QStringList, QString ) ), this, SLOT( messageRcvd( QString, QString, QString, QStringList, QString ) ) );
	connect( irc, SIGNAL( registered() ), this, SLOT( autoRun() ) );
	connect( irc, SIGNAL( sentRAW( QString ) ), this, SLOT( sentRAW( QString ) ) );
	connect( irc, SIGNAL( rcvdRAW( QString ) ), this, SLOT( rcvdRAW( QString ) ) );
	this->setTabPosition( QTabWidget::South );
	_addWindow( "status", "", true );
	
	DCCPortMin = 1024;
	DCCPortMax = 5000;
	
	DCCSendAhead = true;
	
	// Set up variables
	variables["address"] = "$ip";
}

void MessageHandler::_addWindow( QString name, QString defaultCmd, bool focusOnOpen ) {
	if( windows.keys().contains( name ) ) { return; }
	int index = this->addTab( new ChatWindow(), name );
	windows[name] = (ChatWindow*)this->widget( index );
	windows[name]->setDefaultCmd( defaultCmd );
	connect( windows[name], SIGNAL( doCmd( QString ) ), this, SLOT( processCmd( QString ) ) );
	windows[name]->setupUi( windows[name] );
	if( focusOnOpen ) {
		this->setCurrentIndex( index );
		windows[name]->input->setFocus();
	}
}

QString MessageHandler::parseVar( QString text ) {
	if( !text.startsWith( '$' ) ) { return text; }
	QString var = text.right( text.length() - 1 ).toLower();
	QString value;
	
	if( var == "ip" ) { value = irc->getIPAddress(); }
	if( var == "nick" || var == "nickname" ) { value = irc->getNickName(); }
	
	if( value.isNull() && variables.keys().contains( var ) ) { value = variables[var].toString(); }
	return ( value.isNull() || value == text ) ? text : parseVar( value ); // Allows for recursive vars, while avoiding an infinite loop
}

void MessageHandler::messageRcvd( QString type, QString src, QString dest, QStringList values, QString text ) {
	QString window = "status";
	QString source = "server";
	QString message = text;
	
	if( src.startsWith( ':' ) ) { src = src.right( src.length() - 1 ); }
	
	// Handle special messages first
	if( type == "JOIN" ) {
		_addWindow( text, "msg " + text, true );
	}
	
	if( !dest.isEmpty() && dest.startsWith( '#' ) ) {
		window = dest;
	}
	if( !src.isEmpty() && src.contains( '!' ) && src.contains( '@' ) ) {
		QStringList split = src.split( '!' );
		source = split[0];
		if( dest == irc->getNickName() && type == "PRIVMSG" ) {
			window = source;
		}
	}
	
	if( type == "CTCP" ) {
		values = text.split( " " );
		QString ctcpCommand = values.takeFirst().toUpper();
		if( ctcpCommand == "VERSION" ) {
			irc->notice( source, tr( "Atma2 IRC Client %1" ).arg( 0.01 ) );
		}
	}
	
	//emit dispatchMessage( window, src, message );
	_addWindow( window, "msg " + window );
	windows[window]->message( source, message );
}

void MessageHandler::messageSentDCC( QString nickName, QString text ) {
	QString window = nickName.prepend( "=" );
	_addWindow( window, "msg " + window );
	windows[window]->message( irc->getNickName(), text );
}

void MessageHandler::messageRcvdDCC( QString nickName, QString text ) {
	QString window = "=" + nickName;
	_addWindow( window, "msg " + window );
	windows[window]->message( nickName, text );
}

void MessageHandler::processCmd( QString cmdText ) {
	qDebug() << cmdText;

	MIRCScript *script = new MIRCScript(this->scriptManager);
	script->parse(cmdText);
	script->run();
}
void MessageHandler::autoRun() {
	//irc->join( "#pinet" );
}

void MessageHandler::startDCC( QString nickName, QString fileName ) {
	for( int i = DCCPortMin; i <= DCCPortMax; i++ ) {
		if( !DCCServers.keys().contains( i ) ) {
			QString message = "DCC %1";
			if( fileName.isEmpty() ) {
				// Chat session
				DCCServers[i] = new DCCChatServer( nickName );
				connect( ((DCCChatServer*)DCCServers[i]), SIGNAL( connectedChat( QString ) ), this, SLOT( connectedDCCChat( QString ) ) );
				DCCNicks[nickName] = i;
				message = message.arg( "CHAT chat %1 %2" );
			} else {
				// File Send
				QFileInfo fileInfo = QFileInfo( fileName );
				if( !fileInfo.exists() ) { return; }
				DCCServers[i] = new DCCFileServer( fileName, DCCSendAhead );
				message = message.arg( "SEND %1 %3 %2" ).arg( fileInfo.fileName() ).arg( fileInfo.size() ).arg( "%1 %2" );
			}
			bool success = DCCServers[i]->listen( QHostAddress::Any, i );
			qDebug() << QString( "Listen on port %1: %2" ).arg( i ).arg( success );
			message = message.arg( irc->localHost.toIPv4Address() ).arg( i );
			qDebug() << message;
			irc->ctcp( nickName, message );
			break;
		}
	}
}

void MessageHandler::connectedDCCChat( QString nickName ) {
	DCCChatServer *dcc = ((DCCChatServer*)DCCServers[DCCNicks[nickName]]);
	connect( dcc, SIGNAL( rcvdText( QString, QString ) ), this, SLOT( messageRcvdDCC( QString, QString ) ) );
	connect( dcc, SIGNAL( sentText( QString, QString ) ), this, SLOT( messageSentDCC( QString, QString ) ) );
	QString window = nickName.prepend( "=" );
	_addWindow( window, "msg " + window );
	windows[window]->echo( "DCC Chat Connected" );
}

void MessageHandler::sentRAW( QString text ) { qDebug() << ">>> " << text.trimmed(); }
void MessageHandler::rcvdRAW( QString text ) { qDebug() << "<<< " << text.trimmed(); }

bool MessageHandler::call_alias(QString alias, QStringList args) {
	QStringList newargs;
	if( alias == "server" || alias == "connect" ) {
		if( args.count() < 1 ) { return true; }
		irc->quit( tr( "Changing servers" ) );
		QString host = args.takeFirst();
		int port = 0;
		if( host.contains( ':' ) ) {
			QStringList splitHost = host.split( ':' );
			host = splitHost.takeFirst();
			port = splitHost.join( "" ).toInt();
		}
		port = port > 0 ? port : 6667;
		QString nickName = args.count() > 0 ? args.takeFirst() : irc->getNickName();
		QString userName = args.count() > 0 ? args.takeFirst() : irc->getUserName();
		QString realName = args.count() > 0 ? args.join( " " ) : irc->getRealName();
		irc->connectAndRegister(
			host, // Host
			port > 0 ? port : 6667, // Port
			nickName,
			userName,
			realName );
	} else if( alias == "nick" ) {
		if( args.count() < 1 ) { return true; }
		irc->nick( args[0] );
	} else if( alias == "msg" || alias == "privmsg" ) {
		if( args.count() < 2 ) { return true; }
		newargs += args.takeFirst();
		if( newargs[0].startsWith( "=")  ) {
			// DCC Chat
			((DCCChatServer*)DCCServers[DCCNicks[newargs[0].right( newargs[0].length() - 1 )]])->sendText( args.join( " " ) );
		} else {
			irc->msg( newargs[0], args.join( " " ) );
		}
		// Update existing chat windows with the sent message
		QString window;
		QStringList windows = newargs[0].split( "," );
		foreach( window, windows ) {
			if( !window.startsWith( "=" ) && this->windows.keys().contains( window ) ) { this->windows[window]->message( irc->getNickName(), args.join( " " ) ); }
		}
	} else if( alias == "notice" ) {
		if( args.count() < 2 ) { return true; }
		QString dest = args.takeFirst();
		irc->notice( dest, args.join( " " ) );
	} else if( alias == "ctcp" ) {
		if( args.count() < 2 ) { return true; }
		newargs += args.takeFirst();
		irc->ctcp( newargs[0], args.join( " " ) );
	} else if( alias == "join" ) {
		irc->join( args.join( "," ) );
	} else if( alias == "set" ) {
		QString var = args.takeFirst();
		variables[var] = args.join( " " );
	} else if( alias == "dcc" ) {
		if( args.count() < 2 ) { return true; }
		QString type = args.takeFirst();
		QString nick = args.takeFirst();
		if( type == "chat" ) {
			startDCC( nick );
		} else if( type == "send" ) {
			if( args.count() == 0 ) { return true; }
			startDCC( nick, args.join( " " ) );
		}
	} else if( alias == "quit" || alias == "q" ) {
		if( args.count() > 0 ) { irc->quit( args.join( " " ) ); }
		else { irc->quit(); }
	} else if(alias == "echo") {
		windows["status"]->echo(args.join(" "));
	} else {
		//TODO: Script error! Unknown alias.... WHEREVER WE ARE IN WHICHEVER SCRIPT IT IS THAT WE'RE RUNNING!!
		//BUT WAIT!!!!! NO!!!! The script manager is the only thing handling that!!!
		//IN FACT, ALL THE ABOVE ALIASES BELONG IN THE IRC CLIENT LIBRARY, REALLY! PERHAPS?
		return false;
	}
	return true;
}
void MessageHandler::alias_join(QStringList args) {
	irc->join( args.join( "," ) );
}
void MessageHandler::alias_msg(QStringList args) {
}
