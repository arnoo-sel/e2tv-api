#include "demand.h"
#include "search.h"
#include "details.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

Demand::Demand() : method(INVALID)
{
}

Demand Demand::fromArguments(QStringList arguments)
{
	Demand demand;

	arguments.removeFirst();

	if (arguments.first() == "--search")
		demand.method = SEARCH;
	else if (arguments.first() == "--details")
		demand.method = DETAILS;
	else if (arguments.first() == "--sources")
		demand.method = SOURCES;
	else
		return demand;

	arguments.removeFirst();

	for (const QString& arg : arguments)
	{
		if (arg.startsWith("--"))
			demand.options.append(arg);
		else if (demand.query.isEmpty())
			demand.query = arg;
	}

	return demand;
}

void Demand::execute()
{
	if (method == SEARCH)
	{
		Search* search = new Search(*this);
		search->execute();
	}
	else if (method == DETAILS)
	{
		Details* details = new Details(*this);
		details->execute();
	}
	else if (method == SOURCES)
	{
	}
	else
	{
		QJsonObject obj;
		obj.insert("error", QString("invalid method"));
		QJsonDocument doc(obj);
		qDebug() << doc.toJson();
	}
}
