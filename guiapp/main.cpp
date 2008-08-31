#include <QDialog>
#include "../src/ircclient.h"
#include "chatwindow.h"
#include "messagehandler.h"

int main( int argc, char *argv[] ) {
	QApplication app( argc, argv );
	IRCClient *irc = new IRCClient();
	MessageHandler *mh = new MessageHandler( irc );
	
	QObject::connect( &app, SIGNAL( aboutToQuit() ), irc, SLOT( quit() ) );
	mh->show();
	
	irc->connectAndRegister( argc > 1 ? argv[1] : "dev1", 6667, argc > 2 ? argv[2] : "Test", "correlr", "Correl Roush" );
	
	return app.exec();
}

