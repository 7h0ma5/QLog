#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "Conditions.h"

const QUrl FLUX_URL = QUrl("https://services.swpc.noaa.gov/products/summary/10cm-flux.json");
const QUrl K_INDEX_URL = QUrl("https://services.swpc.noaa.gov/products/noaa-planetary-k-index.json");

Conditions::Conditions(QObject *parent) : QObject(parent)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Conditions::update);
    timer->start(30*60*1000);
}

void Conditions::update() {
    nam->get(QNetworkRequest(FLUX_URL));
    nam->get(QNetworkRequest(K_INDEX_URL));
}

void Conditions::processReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (reply->url() == FLUX_URL) {
            QVariantMap obj = doc.object().toVariantMap();
            flux = obj.value("Flux").toInt();
        }
        else if (reply->url() == K_INDEX_URL) {
            k_index = doc.array().last().toArray().at(2).toString().toDouble();
        }
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
