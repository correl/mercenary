#include "mirc.h"

MIRCScriptManager::MIRCScriptManager(QObject *parent) {
	this->parent = parent;
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
	//mirc_script_engine::alias_echo(QStringList("[MANAGER]") << alias << arguments);
	if (internal_aliases.find(alias) != internal_aliases.end()) {
		internal_aliases[alias](arguments);
	}
}

bool MIRCScriptManager::register_alias(QString alias, void (*fn)(QStringList)) {
	internal_aliases[alias] = fn;
}