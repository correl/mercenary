#include <QTextStream>
#include "script.h"

int main() {
	QTextStream output(stdout);

	QString foo;
	foo.append("; Hey, here's some test code\n");
	foo.append("set name Correl\n");
	foo.append("%name = Correl $&\n\tRoush\n");
	foo.append("echo Hello, %name!\n");
	foo.append("alias dostuff {\n");
	foo.append("	; Not very useful, but good for testing the parser!\n");
	foo.append("	var %b = 42\n");
	foo.append("	%b = $calc(%b * 3)\n");
	foo.append("	return %b;\n");
	foo.append("}\n");
	foo.append("alias -l dosomethingelse {\n");
	foo.append("	; Useless local alias!\n");
	foo.append("	echo -s Busy doing nothing\n");
	foo.append("}\n");
	foo.append("alias -l dosomethingelse {\n");
	foo.append("	; Useless local alias!\n");
	foo.append("	echo -s Busy doing nothing\n");
	foo.append("}\n");

	output << "Code:\n"
		<< "================================================================================\n"
		<< foo
		<< "================================================================================\n"
		<< "\n";

	mirc_script_engine *interpreter = new mirc_script_engine();
	mirc_script parser(interpreter);
	
	parse_info<> info = parse((const char*)foo.toLatin1(), parser);

	if (info.full)
	{
		output << "-------------------------\n";
		output << "Parsing succeeded\n";
		output << "-------------------------\n";
		
		MIRCScript *ms = new MIRCScript();
		output << "Creating new MIRCScript object\n";
		//if (ms->parse(foo)) {
		if (ms->load("test.mrc")) {
			output << "MRC LOADED\n";
			output << "Code:\n" << ms->code();
			output << "Aliases:\n";
			QMapIterator<QString, mirc_alias> alias(ms->aliases());
			while(alias.hasNext()) {
				alias.next();
				output << (alias.value().global ? "global" : "local");
				output << " " << alias.key() << "\n";
			}
			if (ms->run()) {
				output << "MRC RAN SUCCESSFULLY\n";
				QMapIterator<QString, QString> i(ms->variables());
				output << "Variables:\n";
				while(i.hasNext()) {
					i.next();
					output << i.key() << " = " << i.value() << "\n";
				}
			}
		}
	}
	else
	{
		output << "-------------------------\n";
		output << "Parsing failed\n";
		output << "stopped at: \": " << info.stop << "\"\n";
		output << "-------------------------\n";
	}
	
	return 0;
}
