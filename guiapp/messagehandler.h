#include <QTabWidget>
#include <QString>
#include <QStringList>
#include <QPointer>
#include "../src/ircclient.h"
#include "../src/dccserver.h"
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
	
	void _addWindow( QString name, QString defaultCmd = "", bool focusOnOpen = false );
	QString parseVar( QString text );
public:
	MessageHandler( IRCClient *irc, QWidget *parent = 0 );
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

