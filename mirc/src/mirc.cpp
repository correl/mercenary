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