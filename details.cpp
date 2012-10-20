#include "details.h"
#include "demand.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <iostream>

Details::Details(const Demand &demand, QObject *parent) :
	QObject(parent), query(demand.query), url("https://www.googleapis.com/freebase/v1/mqlread"), type(INVALID)
{
	url.addQueryItem("indent", "2");

	for (const QString& opt : demand.options)
	{
		if (opt == "--film" && type == INVALID)
			type = FILM;
		else if (opt == "--tv" && type == INVALID)
			type = TV;
	}

	if (demand.options.contains("--film"))
		url.addQueryItem("query", R"*([{ "mid": ")*" + query + R"*(", "name": null,"type": "/film/film", "directed_by": null, "rating": null }])*");
	else if (demand.options.contains("--tv"))
		url.addQueryItem("query", R"*([{ "mid": ")*" + query + R"*(", "name": null, "type": "/tv/tv_program", "seasons": [{ "name": null, "season_number": null, "sort": "season_number", "episodes": [] }] }])*");
}

void Details::execute()
{
	QNetworkAccessManager* manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void Details::replyFinished(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError)
	{
		QJsonObject obj;
		obj.insert("network_error", reply->errorString());

		QJsonDocument doc(obj);
		qDebug() << doc.toJson();
		return;
	}

	QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());

	QJsonDocument listDoc(jsonDoc.object()["result"].toArray().first().toObject());

	std::wcout << QString::fromUtf8(listDoc.toJson()).toStdWString();
	qApp->exit();
}
