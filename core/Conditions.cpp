#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "Conditions.h"

#define FLUX_URL "https://services.swpc.noaa.gov/products/summary/10cm-flux.json"

Conditions::Conditions(QObject *parent) : QObject(parent)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}

void Conditions::update() {
    QUrl url(FLUX_URL);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "QLog/1.0 (Qt)");
    nam->get(request);
}

void Conditions::processReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QVariantMap obj = doc.object().toVariantMap();
        flux = obj.value("Flux").toInt();
        delete reply;
        emit conditionsUpdated();
    }
    else {
        delete reply;
    }

}

Conditions::~Conditions() {
    delete nam;
}
