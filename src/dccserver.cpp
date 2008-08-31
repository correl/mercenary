#include "dccserver.h"

DCCServer::DCCServer( QObject *parent ) : QTcpServer( parent ) {
	connect( this, SIGNAL( newConnection() ), this, SLOT( connectClient() ) );
}

void DCCServer::connectClient() {
	conn = nextPendingConnection();
	connect( conn, SIGNAL( readyRead() ), this, SLOT( readSocket() ) );
	close(); // No more than one client
}

DCCChatServer::DCCChatServer( QString nickName, QObject *parent ) : DCCServer( parent ) {
	this->nickName = nickName.trimmed();
	connect( this, SIGNAL( newConnection() ), this, SLOT( connectChatClient() ) );
}

void DCCChatServer::sendText( QString text ) {
	emit sentText( nickName, text );
	text.append( "\r\n" );
	conn->write( text.toLatin1() );
}

void DCCChatServer::connectChatClient() {
	emit connectedChat( nickName );
}

void DCCChatServer::readSocket() {
	if( !conn->canReadLine() ) return;
	char buf[512];
	while( conn->readLine( buf, sizeof( buf ) ) > 0 )
		emit rcvdText( nickName, QString( buf ).trimmed() );
}

DCCFileServer::DCCFileServer( QString fileName, bool sendAhead, QObject *parent ) : DCCServer( parent ) {
	chunkSize = 1024;
	sent = 0;
	verified = 0;
	this->sendAhead = sendAhead;
	file.setFileName( fileName );
	file.open( QIODevice::ReadOnly );
	connect( this, SIGNAL( newConnection() ), this, SLOT( connectFileClient() ) );
}

void DCCFileServer::connectFileClient() {
	emit connectedFile( conn->localPort() );
	// Send the initial chunk of data
	sendNextChunk();
}

void DCCFileServer::readSocket() {
	char buf[4];
	while( conn->bytesAvailable() >= 4 ) {
		conn->read( buf, 4 );
		quint32 v = *reinterpret_cast<quint32*>( &buf[0] );
		v = qFromBigEndian( v );
		verified = v;
		if( sendAhead || verified == sent ) { sendNextChunk(); }
		if( !file.isOpen() ) { conn->close(); }
	}
}

void DCCFileServer::sendNextChunk() {
	char buf[chunkSize];
	if( !file.isOpen() || !file.isReadable() || !conn->isOpen() ) { return; }
	qint64 bytesRead = 0;
	while( ( sendAhead || sent == verified ) && ( bytesRead = file.read( buf, chunkSize ) ) ) {
		if( bytesRead > 0 ) {
			conn->write( buf, bytesRead );
		}
		sent += bytesRead;
		if( conn->bytesAvailable() >= 4 ) { readSocket(); }
	}
	if( bytesRead < chunkSize && sent == verified ) {
		file.close();
	}
}
