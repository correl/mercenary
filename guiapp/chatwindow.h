#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "ui_chatwindow.h"
#include <QScrollBar>

class ChatWindow : public QWidget, public Ui::ChatWindow {
	Q_OBJECT

private:
	QString filter;
	QString defaultCmd;

public:
	ChatWindow( QWidget *parent = 0 ) : QWidget( parent ) {
	};
	
	void setupUi( QWidget *ChatWindow = 0 ) {
		Ui::ChatWindow::setupUi( ChatWindow );
		connect( input, SIGNAL( returnPressed() ), this, SLOT( processCmd() ) );
	}

public slots:
	void setMessageFilter( QString filter ) { this->filter = filter; }
	void setDefaultCmd( QString cmd ) { this->defaultCmd = cmd; }
	void message( QString src, QString message ) {
		display->append( "<" + src + "> " + message );
		display->verticalScrollBar()->setValue( display->verticalScrollBar()->maximum() );
	}
	void echo( QString text ) {
		display->append( text );
		display->verticalScrollBar()->setValue( display->verticalScrollBar()->maximum() );
	}
	void processCmd() {
		QString cmd = defaultCmd;
		QStringList args = input->text().trimmed().split( " " );
		if( args.count() < 1 ) { return; }
		if( args[0].startsWith( '/' ) ) {
			cmd = args.takeFirst();
			cmd = cmd.right( cmd.length() - 1 );
		}
		emit doCmd( cmd + " " + args.join( " " ) );
		input->clear();
	}

signals:
	void doCmd( QString cmd );
};

#endif

