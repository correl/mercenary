#include "testapp.h"

TestApp::TestApp( int argc, char *argv[] ) : QCoreApplication( argc, argv ) {
	irc = new IRCClient();
	connect( irc->conn, SIGNAL( connected() ), this, SLOT( connected() ) );
	connect( irc->conn, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
	connect( irc, SIGNAL( registered() ), this, SLOT( registered() ) );
	connect( irc, SIGNAL( sentRAW( QString ) ), this, SLOT( sent( QString ) ) );
	connect( irc, SIGNAL( rcvdRAW( QString ) ), this, SLOT( rcvd( QString ) ) );
	connect( irc, SIGNAL( debug( QString ) ), this, SLOT( debug( QString ) ) );
	
	irc->connectAndRegister( argc > 1 ? argv[1] : "dev1", 6667, argc > 2 ? argv[2] : "Test", "correlr", "Correl Roush" );
}

void TestApp::connected() {
	printf( "Connected!\n" );
}

void TestApp::registered() {
	irc->join( "#pinet" );
}

void TestApp::disconnected() {
	printf( "Disconnected.\n" );
}

void TestApp::sent( QString text ) {
	printf( ">>> %s", (const char*)text.toLatin1() );
}

void TestApp::rcvd( QString text ) {
	printf( "<<< %s", (const char*)text.toLatin1() );
}

void TestApp::debug( QString text ) {
	printf( "DEBUG: %s\n", (const char*)text.toLatin1() );
}

