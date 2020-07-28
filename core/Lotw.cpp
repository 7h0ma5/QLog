#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "Lotw.h"
#include "logformat/AdiFormat.h"

#define ADIF_API "https://lotw.arrl.org/lotwuser/lotwreport.adi"

Lotw::Lotw(QObject *parent) : QObject(parent)
{
    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &Lotw::processReply);
}

void Lotw::update(QDate start_date, bool qso_since) {
    QList<QPair<QString, QString>> params;
    params.append(qMakePair(QString("qso_query"), QString("1")));
    params.append(qMakePair(QString("qso_qsldetail"), QString("yes")));

    QString start = start_date.toString("yyyy-MM-dd");
    if (qso_since) {
        params.append(qMakePair(QString("qso_qsl"), QString("no")));
        params.append(qMakePair(QString("qso_qsorxsince"), start));
    }
    else {
        params.append(qMakePair(QString("qso_qsl"), QString("yes")));
        params.append(qMakePair(QString("qso_qslsince"), start));
    }

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
        reply->deleteLater();
        emit updateFailed();
        return;
    }

    qint64 size = reply->size();
    qDebug() << "Reply received, size: " << size;

    emit updateStarted();

    QTextStream stream(reply);
    AdiFormat adi(stream);

    QSqlDatabase db(QSqlDatabase::database());
    QVariantMap contact;

    QSqlQuery query(db);
    query.prepare("SELECT id, lotw_qsl_sent, lotw_qsl_rcvd, lotw_qslrdate, iota, cnty, state, gridsquare FROM contacts"
                  " WHERE callsign = :callsign AND band = :band AND mode = :mode"
                  " AND (start_time BETWEEN :start_time_min AND :start_time_max)"
                  " LIMIT 1");

    QSqlQuery update_status_query(db);
    update_status_query.prepare("UPDATE contacts SET lotw_qsl_sent = :lotw_qsl_sent, lotw_qsl_rcvd = :lotw_qsl_rcvd,"
                                " lotw_qslrdate = :lotw_qslrdate"
                                " WHERE id = :id");

    QSqlQuery update_iota_query(db);
    update_iota_query.prepare("UPDATE contacts SET iota = :value WHERE id = :id");

    QSqlQuery update_cnty_query(db);
    update_cnty_query.prepare("UPDATE contacts SET cnty = :value WHERE id = :id");

    QSqlQuery update_state_query(db);
    update_state_query.prepare("UPDATE contacts SET state = :value WHERE id = :id");

    QSqlQuery update_gridsquare_query(db);
    update_gridsquare_query.prepare("UPDATE contacts SET gridsquare = :value WHERE id = :id");

    LotwUpdate status = { 0, 0, 0, 0 };

    while (adi.readContact(contact)) {
        status.qsos_checked++;

        QDate date_on = AdiFormat::parseDate(contact.value("qso_date").toString());
        QTime time_on = AdiFormat::parseTime(contact.take("time_on").toString());
        QDateTime start_time_min = QDateTime(date_on, time_on, Qt::UTC).addSecs(-10*60);
        QDateTime start_time_max = QDateTime(date_on, time_on, Qt::UTC).addSecs(10*60);

        query.bindValue(":callsign", contact.value("call").toString());
        query.bindValue(":band", contact.value("band").toString().toLower());
        query.bindValue(":mode", contact.value("mode").toString().toUpper());
        query.bindValue(":start_time_min", start_time_min);
        query.bindValue(":start_time_max", start_time_max);
        query.exec();

        if (query.next()) {
            QDate qslrdate;

            if (!contact.value("qslrdate").toString().isEmpty()) {
                qslrdate = AdiFormat::parseDate(contact.value("qslrdate").toString());
            }

            if (query.value(1) != "Y" || query.value(2) != contact.value("qsl_rcvd") || query.value(3).toDate() != qslrdate) {
                update_status_query.bindValue(":lotw_qsl_sent", "Y");
                update_status_query.bindValue(":lotw_qsl_rcvd", contact.value("qsl_rcvd"));
                update_status_query.bindValue(":lotw_qslrdate", qslrdate);
                update_status_query.bindValue(":id", query.value(0));
                update_status_query.exec();

                qDebug() << query.value(1) << query.value(2) << contact.value("qsl_rcvd") << query.value(3) << qslrdate;

                status.qsos_updated++;
                if (query.value(2) != contact.value("qsl_rcvd")) {
                    status.qsls_updated++;
                }
            }

            if (!contact.value("iota").toString().isEmpty() && query.value(4).toString().isEmpty()) {
                update_iota_query.bindValue(":value", contact.value("iota"));
                update_iota_query.bindValue(":id", query.value(0));
                update_iota_query.exec();
            }

            if (!contact.value("cnty").toString().isEmpty() && query.value(5).toString().isEmpty()) {
                update_cnty_query.bindValue(":value", contact.value("cnty"));
                update_cnty_query.bindValue(":id", query.value(0));
                update_cnty_query.exec();
            }

            if (!contact.value("state").toString().isEmpty() && query.value(6).toString().isEmpty()) {
                update_state_query.bindValue(":value", contact.value("state"));
                update_state_query.bindValue(":id", query.value(0));
                update_state_query.exec();
            }

            if (!contact.value("gridsquare").toString().isEmpty() && query.value(7).toString().isEmpty()) {
                update_gridsquare_query.bindValue(":value", contact.value("gridsquare"));
                update_gridsquare_query.bindValue(":id", query.value(0));
                update_gridsquare_query.exec();
            }
        }
        else {
            qDebug() << "Not Found! " << contact.value("call").toString() << contact.value("qso_date").toString();
            status.qsos_unmatched++;
        }

        contact.clear();
        if (size > 0) {
            double progress = static_cast<double>(stream.pos()) * 100.0 / static_cast<double>(size);
            emit updateProgress(static_cast<int>(progress));
        }
    }

    reply->deleteLater();
    emit updateComplete(status);
}
