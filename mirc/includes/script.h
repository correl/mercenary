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
	MIRCScriptManager *manager;
	QString script;
	QMap<QString, mirc_alias> _aliases;
	QMap<QString, QString> _variables;
	mirc_script_engine *interpreter;
	mirc_script *parser;
	iterator_t *begin;
	iterator_t *end;
	bool loaded;
public:
	MIRCScript(MIRCScriptManager *m);
	bool load(QString filename);
	bool parse(QString script);
	bool run();
	bool run(QString alias);
	QString code();
	QString code(QString alias);
	int line();
	QMap<QString, mirc_alias> aliases();
	QMap<QString, QString> variables();
};

#endif
