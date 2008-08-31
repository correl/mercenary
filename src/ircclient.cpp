#include "ircclient.h"
#include <QDebug>

IRCClient::IRCClient( QObject *parent ) : QObject( parent ) {
	conn = new QTcpSocket();
	QObject::connect( conn, SIGNAL( readyRead() ), this, SLOT( readSocket() ) );
	QObject::connect( conn, SIGNAL( connected() ), this, SLOT( _connected() ) );
	QObject::connect( conn, SIGNAL( disconnected() ), this, SLOT( _disconnected() ) );	
	QObject::connect( this, SIGNAL( rcvdRAW( QString ) ), this, SLOT( parseMessage( QString ) ) );
	nickName = userName = realName = "";
}

void IRCClient::connectAndRegister( QString host, uint port, QString nickName, QString userName, QString realName ) {
	conn->abort();
    conn->connectToHost( host, port );
	this->nickName = nickName;
	this->userName = userName;
	this->realName = realName;
}

void IRCClient::raw( QString text ) {
	// Send raw text to the IRC Server
	text.append( "\r\n" );
	conn->write( text.toLatin1() );
	emit sentRAW( text );
}

void IRCClient::msg( QString dest, QString text ) { raw( "PRIVMSG " + dest + " :" + text ); }
void IRCClient::ctcp( QString dest, QString text ) { raw( "PRIVMSG " + dest + " :\001" + text + "\001" ); }
void IRCClient::notice( QString dest, QString text ) { raw( "NOTICE " + dest + " :" + text ); }
void IRCClient::join( QString channel ) { raw( "JOIN " + channel ); }
void IRCClient::quit( QString reason ) { raw( "QUIT :" + reason ); conn->flush(); }
void IRCClient::quit() { quit( tr( "Quit" ) ); }
void IRCClient::nick( QString nick ) { raw( "NICK " + nick ); }

void IRCClient::_connected() {
	localHost = conn->localAddress();
	emit connected();
	// Register
	raw( "USER " + userName + " +iw localhost :" + realName );
	nick( nickName );
}
void IRCClient::_disconnected() { emit disconnected(); }

void IRCClient::closeSocket() {
	emit disconnected();
}

void IRCClient::updateHostInfo( QHostInfo host ) {
	if (host.error() != QHostInfo::NoError) {
		qDebug() << "Lookup failed: " + host.errorString();
		return;
	}

	foreach (QHostAddress address, host.addresses())
		localHost = address;
	qDebug() << "Host updated: " + localHost.toString();
}

void IRCClient::readSocket() {
	if( !conn->canReadLine() ) return;
	char buf[512];
	while( conn->readLine( buf, sizeof( buf ) ) > 0 )
		emit rcvdRAW( buf );
}

void IRCClient::parseMessage( QString text ) {
	text = text.trimmed();
	int split = text.indexOf( " :" );
	QString message = split >= 0 ? text.left( split ) : text;
	QString messageText = split >= 0 ? text.right( text.length() - split - 2 ) : "";
	
	QString from = "";
	QString type = "";
	QString dest = "";
	QStringList values;
	QStringList parsedMessage = message.split( ' ' );
	if( parsedMessage.count() > 0 ) {
		if( message.startsWith( ':' ) ) from = parsedMessage.takeFirst();
		if( parsedMessage.count() > 0 ) type = parsedMessage.takeFirst();
		if( parsedMessage.count() > 0 ) dest = parsedMessage.takeFirst();
		values = parsedMessage;
	} else {
		type = message;
	}
	
	// The following are special cases that can be taken care of immediately
	// Anything else should probably be dealt with by an actual client
	if( type == "001" ) {
		emit registered();
		// Fetch the hostname from the welcome text if it's there and perform a nonblocking dns lookup
		QString hostMask = messageText.right( messageText.length() - messageText.lastIndexOf( ' ' ) - 1 );
		if( hostMask.contains( '@' ) ) {
			QString hostName = hostMask.right( hostMask.length() - hostMask.indexOf( '@' ) - 1 );
			qDebug() << "Looking up " + hostName;
			QHostInfo::lookupHost( hostName, this, SLOT( updateHostInfo( QHostInfo ) ) );
		} else {
			raw( "WHOIS " + nickName );
		}
	} else if( type == "311" ) {
		// RPL_WHOIS
		if( values.count() < 3 ) return;
		if( values[0] == nickName ) {
			qDebug() << "Looking up " + values[2];
			QHostInfo::lookupHost( values[2], this, SLOT( updateHostInfo( QHostInfo ) ) );
		}
	} else if( type == "NICK" && from.startsWith( ":" + nickName + "!" ) ) {
		nickName = messageText;
	} else if( type == "PING" ) {
		raw( "PONG :" + messageText );
		emit pingPong();
	} else if( type == "PRIVMSG" ) {
		if( messageText.startsWith( "\001" ) && messageText.endsWith( "\001" ) ) {
			// CTCP Message
			type = "CTCP";
			messageText = messageText.mid( 1, messageText.length() - 2 );
		}
	}
	//qDebug() << "t" << type << "s" << from << "d" << dest << "v" << values.join( "," ) << "m" << messageText;
	emit messageRcvd( type, from, dest, values, messageText );
}
