#include "mirc.h"

MIRCScriptManager::MIRCScriptManager(QObject *parent) {
	this->parent = parent;
	line_offset = 0;

	register_alias("line", bind(&MIRCScriptManager::_alias_line, this, _1));
	register_alias("lower", bind(&MIRCScriptManager::_alias_lower, this, _1));
	register_alias("return", bind(&MIRCScriptManager::_alias_return, this, _1));
	register_alias("version", bind(&MIRCScriptManager::_alias_version, this, _1));
}

bool MIRCScriptManager::load(QString filename) {
	MIRCScript *script = new MIRCScript(this);
	if (script->load(filename)) {
		scripts << script;
		current_script = script;
		script->run();
		return true;
	}
	return false;
}

bool MIRCScriptManager::hasVariable(QString variable) {
	return _variables.contains(variable);
}

QString MIRCScriptManager::variable(QString variable) {
	return _variables.value(variable);
}

void MIRCScriptManager::variable(QString variable, QString value) {
	_variables[variable] = value;
}

QMap<QString, QString> MIRCScriptManager::variables() {
	return _variables;
}

void MIRCScriptManager::call_alias(QString alias, QStringList arguments) {
	// This is where we flip through and attempt to call an alias
	if (internal_aliases.find(alias) != internal_aliases.end()) {
		internal_aliases[alias](arguments);
	} else {
		// Search scripted aliases
		bool found = false;
		int script_index = 0;
		for (int i = 0; i < scripts.size(); i++) {
			QMap<QString, mirc_alias> aliases = scripts.at(i)->aliases();
			if (aliases.find(alias) != aliases.end()) {
				found = true;
				script_index = i;
				break;
			}
		}
		if (found) {
			MIRCScript *s = new MIRCScript(this);
			MIRCScript *p = current_script;
			int offset = line_offset;
			line_offset = scripts.at(script_index)->aliases()[alias].line;
			current_script = s;
			QString code = scripts.at(script_index)->code(alias);
			if (s->parse(code)) {
				s->run();
			} else {
				qDebug() << "SYNTAX ERROR IN " << alias;
				return_value("");
			}
			current_script = p;
			line_offset = offset;
			delete s;
		} else {
			qDebug() << "UNKNOWN ALIAS" << alias;
			return_value("");
			emit unknown_alias(alias, arguments);
		}
	}
}
QString MIRCScriptManager::return_value() {
	return _return_value;
}
void MIRCScriptManager::return_value(QString value) {
	_return_value = value;
}
bool MIRCScriptManager::register_alias(QString alias, _MIRCAliasHandler fn) {
	internal_aliases[alias] = fn;
}

// Built-in aliases

void MIRCScriptManager::_alias_line(QStringList) {
	return_value(QString::number(current_script->line() + line_offset));
}
void MIRCScriptManager::_alias_lower(QStringList arguments) {
	return_value(arguments.join(" ").toLower());
}
void MIRCScriptManager::_alias_return(QStringList arguments) {
	return_value(arguments.join(" "));
	//TODO: Somehow terminate the currently running script
}
void MIRCScriptManager::_alias_version(QStringList) {
	return_value(MIRC_VERSION);
}
