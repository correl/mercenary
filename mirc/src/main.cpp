#include <QTextStream>
#include "script.h"

void alias_echo(QStringList arguments) {
	QTextStream output(stdout);
	output << "[ECHO] " << arguments.join(" ") << "\n";
}

int main(int argc, char* argv[]) {
	QTextStream output(stdout);

	output << "* Creating manager\n";
	MIRCScriptManager *mirc = new MIRCScriptManager;
	mirc->register_alias("echo", &alias_echo);
	output << "* Creating script object\n";
	MIRCScript *ms = new MIRCScript(mirc);
	if (argc < 2) {
		output << "! No mIRC script file was specified!\n";
		return(1);
	}
	output << "* Attempting to load " << argv[1] << "\n";
	if (ms->load(argv[1])) {
		output << "* MRC LOADED\n";
		output << "Code:\n" << ms->code();
		output << "Aliases:\n";
		QMapIterator<QString, mirc_alias> alias(ms->aliases());
		while(alias.hasNext()) {
			alias.next();
			output << (alias.value().global ? "global" : "local");
			output << " " << alias.key() << "()\n";
		}
		output << "---\n";
		if (ms->run()) {
			output << "\n---\nMRC RAN SUCCESSFULLY\n";
			QMapIterator<QString, QString> i(mirc->variables());
			output << "Variables:\n";
			while(i.hasNext()) {
				i.next();
				output << i.key() << " = " << i.value() << "\n";
			}
			output << "Aliases:\n";
			QMapIterator<QString, mirc_alias> alias(ms->aliases());
			while(alias.hasNext()) {
				alias.next();
				output << (alias.value().global ? "global" : "local");
				output << " " << alias.key() << "()\n";
			}
		}
	} else {
		output << "Failed to load " << argv[1] << "\n";
		return(1);
	}
	return 0;
}
