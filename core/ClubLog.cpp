#include <QSettings>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "logformat/AdiFormat.h"
#include "ClubLog.h"

#define API_KEY "21507885dece41ca049fec7fe02a813f2105aff2"
#define API_LIVE_UPLOAD_URL "https://clublog.org/realtime.php"

ClubLog::ClubLog(QObject *parent) : QObject(parent) {
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}

void ClubLog::uploadContact(QSqlRecord record) {
    QSettings settings;
    QString email = settings.value("clublog/email").toString();
    QString callsign = settings.value("clublog/callsign").toString();
    QString password = settings.value("clublog/password").toString();

    if (email.isEmpty() || callsign.isEmpty() || password.isEmpty()) {
        return;
    }

    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    AdiFormat adi(stream);
    adi.exportContact(record);
    stream.flush();

    QUrlQuery query;
    query.addQueryItem("email", email);
    query.addQueryItem("callsign", callsign);
    query.addQueryItem("password", password);
    query.addQueryItem("api", API_KEY);
    query.addQueryItem("adif", data);

    QUrl url(API_LIVE_UPLOAD_URL);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    nam->post(request, query.query().toUtf8());
}

void ClubLog::processReply(QNetworkReply* reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "ClubLog error" << reply->errorString();
        delete reply;
        return;
    }
    else {
        qDebug() << "ClubLog update sent.";
        delete reply;
        return;
    }
}
