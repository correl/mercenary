#ifndef IRCCLIENT_H
#define IRCCLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include <QHostAddress>
#include <QHostInfo>


class IRCClient : public QObject {
	Q_OBJECT
private:
	QString nickName, userName, realName;

public:
	QTcpSocket *conn;
	QHostAddress localHost;
	
	IRCClient( QObject *parent = 0 );
	//void connect( QString host, uint port, QString nickName );
	void connectAndRegister( QString host, uint port, QString nickName, QString userName, QString realName );
	void raw( QString text );
	void msg( QString dest, QString message );
	void ctcp( QString dest, QString message );
	void notice( QString dest, QString message );
	void join( QString channel );
	void nick( QString nick );
	
	QString getNickName() { return nickName; }
	QString getUserName() { return userName; }
	QString getRealName() { return realName; }
	QString getIPAddress() { return localHost.toString(); }

signals:
	// Basic
	void connected();
	void disconnected();
	void registered();
	
	// Debug
	void rcvdRAW( QString text );
	void sentRAW( QString text );
	void debug( QString text );
	
	// Events
	void pingPong();
	void messageRcvd( QString type, QString src, QString dest, QStringList values, QString text );

public slots:
	void quit( QString reason );
	void quit(); // No reason...
	void updateHostInfo( QHostInfo host );

private slots:
	void _connected();
	void _disconnected();
	void closeSocket();
	void readSocket();
	void parseMessage( QString text );
};

#endif

