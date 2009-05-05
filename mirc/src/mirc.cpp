#include "mirc.h"

MIRCScriptManager::MIRCScriptManager(QObject *parent) {
	this->parent = parent;
	line_offset = 0;
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
		// Handle some builtins
		if (alias == "line") {
			return_value(QString::number(current_script->line() + line_offset));
		} else if (alias == "lower") {
			return_value(arguments.join(" ").toLower());
		} else if (alias == "return") {
			return_value(arguments.join(" "));
			//TODO: Somehow terminate the currently running script
		} else if (alias == "version") {
			return_value(MIRC_VERSION);
		} else {
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
				}
				current_script = p;
				line_offset = offset;
				delete s;
			} else {
				qDebug() << "UNKNOWN ALIAS" << alias;
				emit unknown_alias(alias, arguments);
			}
		}
	}
}
QString MIRCScriptManager::return_value() {
	return _return_value;
}
void MIRCScriptManager::return_value(QString value) {
	_return_value = value;
}
bool MIRCScriptManager::register_alias(QString alias, void (*fn)(QStringList)) {
	internal_aliases[alias] = fn;
}
