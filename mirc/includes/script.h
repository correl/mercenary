#ifndef MIRC_SCRIPT_H
#define MIRC_SCRIPT_H

#include <QObject>
#include <QString>
#include <QMap>
#include "mirc.h"
#include "parser.h"

class MIRCScript : public QObject {
	Q_OBJECT
private:
	QString script;
	QMap<QString, mirc_alias> _aliases;
	QMap<QString, QString> _variables;
	bool loaded;
public:
	MIRCScript();
	bool load(QString filename);
	bool parse(QString script);
	bool run();
	bool run(QString alias);
	QString code();
	QString code(QString alias);
	QMap<QString, mirc_alias> aliases();
	QMap<QString, QString> variables();
};

#endif
