#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include "Lotw.h"
#include "logformat/AdiFormat.h"

#define ADIF_API "https://lotw.arrl.org/lotwuser/lotwreport.adi"

Lotw::Lotw(QObject *parent) : QObject(parent)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &Lotw::processReply);
}

void Lotw::update() {
    QList<QPair<QString, QString>> params;
    params.append(qMakePair(QString("qso_query"), QString("1")));
    params.append(qMakePair(QString("qso_qsl"), QString("no")));
    params.append(qMakePair(QString("qso_qsodetail"), QString("yes")));
    get(params);
}

void Lotw::get(QList<QPair<QString, QString>> params) {
    QSettings settings;
    QString username = settings.value("lotw/username").toString();
    QString password = settings.value("lotw/password").toString();

    QUrlQuery query;
    query.setQueryItems(params);
    query.addQueryItem("login", username);
    query.addQueryItem("password", password);

    QUrl url(ADIF_API);
    url.setQuery(query);

    qDebug() << url.toString();

    nam->get(QNetworkRequest(url));
}

void Lotw::processReply(QNetworkReply* reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "LotW error" << reply->errorString();
        delete reply;
        return;
    }

    QTextStream stream(reply);
    AdiFormat adi(stream);

    QVariantMap contact;

    while (adi.readContact(contact)) {
        qDebug() << contact;
    }

    qDebug() << QString(reply->readAll());

    delete reply;
}
