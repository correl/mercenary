#include <iostream>
#include <QCoreApplication>
#include "../src/ircclient.h"

class TestApp : public QCoreApplication {
	Q_OBJECT

public:
	TestApp( int argc, char *argv[] );

private:
	IRCClient *irc;

private slots:
	void connected();
	void disconnected();
	void registered();
	void sent( QString text );
	void rcvd( QString text );
	void debug( QString text );
};
