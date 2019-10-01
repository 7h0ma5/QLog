#include <QSettings>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include "logformat/AdiFormat.h"
#include "ClubLog.h"

#define API_KEY "21507885dece41ca049fec7fe02a813f2105aff2"
#define API_LIVE_UPLOAD_URL "https://clublog.org/realtime.php"
#define API_LOG_UPLOAD_URL "https://clublog.org/putlogs.php"

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

void ClubLog::uploadAdif(QByteArray& data) {
    QSettings settings;
    QString email = settings.value("clublog/email").toString();
    QString callsign = settings.value("clublog/callsign").toString();
    QString password = settings.value("clublog/password").toString();

    if (email.isEmpty() || callsign.isEmpty() || password.isEmpty()) {
        return;
    }

    QUrl url(API_LOG_UPLOAD_URL);

    QHttpMultiPart* multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart emailPart;
    emailPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"email\""));
    emailPart.setBody(email.toUtf8());

    QHttpPart callsignPart;
    callsignPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"callsign\""));
    callsignPart.setBody(callsign.toUtf8());

    QHttpPart passwordPart;
    passwordPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
    passwordPart.setBody(password.toUtf8());

    QHttpPart clearPart;
    clearPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"clear\""));
    clearPart.setBody("0");

    QHttpPart apiPart;
    apiPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"api\""));
    apiPart.setBody(API_KEY);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"clublog.adi\""));
    filePart.setBody(data);

    multipart->append(emailPart);
    multipart->append(passwordPart);
    multipart->append(callsignPart);
    multipart->append(clearPart);
    multipart->append(apiPart);
    multipart->append(filePart);

    qDebug() << multipart->boundary();

    QNetworkRequest request(url);
    QNetworkReply* reply = nam->post(request, multipart);
    multipart->setParent(reply);
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
