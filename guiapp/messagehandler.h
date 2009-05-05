#include <QTabWidget>
#include <QString>
#include <QStringList>
#include <QPointer>
#include <ircclient.h>
#include <dccserver.h>
#include <mirc.h>
#include "chatwindow.h"

class MessageHandler : public QTabWidget {
	Q_OBJECT
private:
	IRCClient *irc;
	QHash< QString, ChatWindow* > windows;
	QHash< QString, QVariant > variables;
	int DCCPortMin;
	int DCCPortMax;
	bool DCCSendAhead;
	QHash< int, DCCServer* > DCCServers;
	QHash< QString, int > DCCNicks;
	MIRCScriptManager *scriptManager;
	
	void _addWindow( QString name, QString defaultCmd = "", bool focusOnOpen = false );
	QString parseVar( QString text );
public:
	MessageHandler( IRCClient *irc, QWidget *parent = 0 );

	void alias_connect(QStringList args);
	void alias_ctcp(QStringList args);
	void alias_dcc(QStringList args);
	void alias_echo(QStringList args);
	void alias_ip(QStringList args);
	void alias_join(QStringList args);
	void alias_msg(QStringList args);
	void alias_nick(QStringList args);
	void alias_notice(QStringList args);
	void alias_say(QStringList args);
	void alias_quit(QStringList args);
public slots:
	void messageRcvd( QString type, QString src, QString dest, QStringList values, QString text );
	void messageSentDCC( QString nickName, QString text );
	void messageRcvdDCC( QString nickName, QString text );
	void processCmd( QString cmdText );
	void autoRun();
	void startDCC( QString nickName, QString fileName = "" );
	void connectedDCCChat( QString nickName );
	
	void rcvdRAW( QString text );
	void sentRAW( QString text );
signals:
	void dispatchMessage( QString window, QString src, QString message );
};

