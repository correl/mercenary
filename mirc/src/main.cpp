#include <QTextStream>
#include <QDebug>
#include "script.h"

void alias_echo(QStringList arguments) {
	qDebug() << "[ECHO] " << arguments.join(" ");
}

int main(int argc, char* argv[]) {
//	qDebug() << "* Creating manager\n";
	MIRCScriptManager *mirc = new MIRCScriptManager;
	mirc->register_alias("echo", &alias_echo);

	if (argc < 2) {
		qDebug() << "! No mIRC script file was specified!\n";
		return(1);
	}
//	qDebug() << "* Attempting to load " << argv[1] << "\n";
	if (mirc->load(argv[1])) {
		// ??
	} else {
		qDebug() << "Failed to load " << argv[1] << "\n";
		return(1);
	}
	return 0;
}
