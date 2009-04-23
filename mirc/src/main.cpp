#include <QTextStream>
#include "script.h"

QTextStream output(stdout);

void alias_echo(QStringList arguments) {
	output << "[ECHO] " << arguments.join(" ") << "\n";
}

int main(int argc, char* argv[]) {
	output << "* Creating manager\n";
	MIRCScriptManager *mirc = new MIRCScriptManager;
	mirc->register_alias("echo", &alias_echo);

	if (argc < 2) {
		output << "! No mIRC script file was specified!\n";
		return(1);
	}
	output << "* Attempting to load " << argv[1] << "\n";
	if (mirc->load(argv[1])) {
		// ??
	} else {
		output << "Failed to load " << argv[1] << "\n";
		return(1);
	}
	return 0;
}
