#ifndef DCCSERVER_H
#define DCCSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QtEndian>
#include <QFile>

class DCCServer: public QTcpServer {
	Q_OBJECT
protected:
	QTcpSocket *conn;
public:
	DCCServer( QObject *parent = 0 );
protected slots:
	void connectClient();
};

class DCCChatServer : public DCCServer {
	Q_OBJECT
private:
	QString nickName;
public:
	DCCChatServer( QString nickName, QObject *parent = 0 );
private slots:
	void connectChatClient();
	void readSocket();
public slots:
	void sendText( QString text );
signals:
	void connectedChat( QString nickName );
	void sentText( QString nickName, QString text );
	void rcvdText( QString nickName, QString text );
};

class DCCFileServer : public DCCServer {
	Q_OBJECT
private:
	int port;
	QFile file;
	int chunkSize;
	quint64 sent;
	quint64 verified;
	bool sendAhead;
public:
	DCCFileServer( QString fileName, bool sendAhead = false, QObject *parent = 0 );
private slots:
	void connectFileClient();
	void readSocket();
	void sendNextChunk();
signals:
	void connectedFile( int port );
	void progress( int port, double percentage );
	void complete( int port, bool success );
};

#endif

