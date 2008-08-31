#ifndef MIRC_H
#define MIRC_H

#include <QObject>
#include <QString>
#include <QMap>
#include "script.h"

class MIRCScriptManager : public QObject {
	Q_OBJECT

public:
	//QMap<QString, QString> variables;
	
	MIRCScriptManager(QObject *parent);
};

#endif
