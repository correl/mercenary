#include "script.h"
#include <QFile>

MIRCScript::MIRCScript(MIRCScriptManager *m) {
	manager = m;
}

bool MIRCScript::load(QString filename) {
	QFile file(filename);
	if (file.exists() && file.open(QIODevice::ReadOnly)) {
		QString code(file.readAll());
		file.close();
		return parse(code);
	} else {
		return false;
	}
}

bool MIRCScript::parse(QString code) {
	mirc_script_engine *interpreter = new mirc_script_engine(manager);
	mirc_script parser(interpreter);
	parse_info<> info = boost::spirit::parse((const char*)code.toLatin1(), parser);
	loaded = info.full;
	if (loaded) {
		script = interpreter->script.code;
		_aliases = interpreter->aliases;
	}
	return loaded;
}

bool MIRCScript::run() {
	if (!loaded) return false;
	mirc_script_engine *interpreter = new mirc_script_engine(manager);
	interpreter->stage = EXECUTE;
	mirc_script parser(interpreter);
	parse_info<> info = boost::spirit::parse((const char*)script.toLatin1(), parser);
	if (info.full) {
		_variables = interpreter->vars;
	}
	return info.full;
}

bool MIRCScript::run(QString alias) {
	if (!loaded) return false;
	return false;
}

QString MIRCScript::code() {
	return script;
}

QString MIRCScript::code(QString alias) {
	return QString();
}

QMap<QString, mirc_alias> MIRCScript::aliases() {
	return _aliases;
}

QMap<QString, QString> MIRCScript::variables() {
	return _variables;
}
