#ifndef MIRC_H
#define MIRC_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include "script.h"

class MIRCScript;

class MIRCScriptManager : public QObject {
	Q_OBJECT
private:
	QObject *parent;
	QVector<MIRCScript> scripts;
	QMap<QString, QString> _variables;
public:
	MIRCScriptManager(QObject *parent = 0);
	/*
	bool load(QString filename);
	bool unload(QString filename);
	
	QString call_alias(QString alias, QStringList arguments);
	*/
	bool hasVariable(QString variable);
	QString variable(QString variable);
	void variable(QString variable, QString value);
	QMap<QString, QString> variables();
};

#endif
