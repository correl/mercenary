#ifndef MIRC_H
#define MIRC_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <QTextStream>
#include "script.h"
#include "parser.h"

class MIRCScript;

class MIRCScriptManager : public QObject {
	Q_OBJECT
private:
	QObject *parent;
	QVector<MIRCScript> scripts;
	QMap<QString, QString> _variables;
	QMap<QString,void (*)(QStringList)> internal_aliases;
	QMap<QString, mirc_alias> aliases;
	QTextStream *output;
public:
	MIRCScriptManager(QObject *parent = 0);
	/*
	bool load(QString filename);
	bool unload(QString filename);
	*/
	void call_alias(QString alias, QStringList arguments);
	bool register_alias(QString alias, void (*fn)(QStringList));

	bool hasVariable(QString variable);
	QString variable(QString variable);
	void variable(QString variable, QString value);
	QMap<QString, QString> variables();
};

#endif
