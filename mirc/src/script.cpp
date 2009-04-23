#include "script.h"
#include <QFile>

MIRCScript::MIRCScript(MIRCScriptManager *m) {
	manager = m;
	interpreter = new mirc_script_engine(manager);
	parser = new mirc_script(interpreter);
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
	parse_info<> info = boost::spirit::parse((const char*)code.toLatin1(), *parser);
	loaded = info.full;
	if (loaded) {
		script = interpreter->script.code;
		_aliases = interpreter->aliases;
	}
	return loaded;
}

bool MIRCScript::run() {
	if (!loaded) return false;
	interpreter->stage = EXECUTE;
	parse_info<> info = boost::spirit::parse((const char*)script.toLatin1(), *parser);
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
	if (_aliases.find(alias) != _aliases.end()) {
		return _aliases[alias].code;
	} else {
		return QString();
	}
}

QMap<QString, mirc_alias> MIRCScript::aliases() {
	return _aliases;
}

QMap<QString, QString> MIRCScript::variables() {
	return _variables;
}
