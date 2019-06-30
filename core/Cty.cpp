#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDebug>
#include <QStringRef>
#include <QDir>
#include <QFile>
#include <QSqlTableModel>
#include <QSqlRecord>
#include "Cty.h"

#define CTY_URL "http://www.country-files.com/cty/cty.csv"

Cty::Cty() {
}

void Cty::update() {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (dir.exists("cty.csv")) {
        qDebug() << "use cached cty.csv at" << dir.path();
        QTimer::singleShot(0, this, &Cty::loadData);
    }
    else {
        download();
    }
}

void Cty::loadData() {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile file(dir.filePath("cty.csv"));
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    parseData(stream);
    file.close();

    emit finished(true);
}

void Cty::download() {
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    QUrl url(CTY_URL);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "QLog/1.0 (Qt)");
    nam->get(request);
    qDebug() << "download cty.csv from" << url.toString();
}

void Cty::processReply(QNetworkReply* reply) {
    QByteArray data = reply->readAll();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << reply->header(QNetworkRequest::KnownHeaders::LocationHeader);

        QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

        QFile file(dir.filePath("cty.csv"));
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.flush();
        file.close();

        delete reply;
        delete nam;

        nam = nullptr;

        loadData();
    }
    else {
        qDebug() << "failed to download cty.csv";

        delete reply;
        delete nam;

        nam = nullptr;

        emit finished(false);
    }

}

void Cty::parseData(QTextStream& data) {
    qDebug() << "Parse cty.csv data";
    QRegExp prefixSeperator("[\\s;]");
    QRegExp prefixFormat("(=?)([A-Z0-9/]+)(?:\\((\\d+)\\))?(?:\\[(\\d+)\\])?$");

    QSqlTableModel entityTableModel;
    entityTableModel.setTable("dxcc_entities");
    QSqlRecord entityRecord = entityTableModel.record();

    QSqlTableModel prefixTableModel;
    prefixTableModel.setTable("dxcc_prefixes");
    prefixTableModel.removeColumn(prefixTableModel.fieldIndex("id"));
    QSqlRecord prefixRecord = prefixTableModel.record();

    int count = 0;

    while (!data.atEnd()) {
        QString line = data.readLine();
        QStringList fields = line.split(',');

        if (fields.count() != 10)  {
            qDebug() << "Invalid line in cty.csv";
            continue;
        }
        else if (fields.at(0).startsWith("*")) {
            continue;
        }

        int dxcc_id = fields.at(2).toInt();

        entityRecord.clearValues();
        entityRecord.setValue("id", dxcc_id);
        entityRecord.setValue("prefix", fields.at(0));
        entityRecord.setValue("name", fields.at(1));
        entityRecord.setValue("cont", fields.at(3));
        entityRecord.setValue("cqz", fields.at(4));
        entityRecord.setValue("ituz", fields.at(5));
        entityRecord.setValue("lat", fields.at(6).toFloat());
        entityRecord.setValue("lon", -fields.at(7).toFloat());
        entityRecord.setValue("tz", fields.at(8).toFloat());
        entityTableModel.insertRecord(-1, entityRecord);
        entityTableModel.submitAll();

        QStringList prefixList = fields.at(9).split(prefixSeperator, QString::SkipEmptyParts);
        for (QString prefix : prefixList) {
            if (prefixFormat.exactMatch(prefix)) {
                prefixRecord.clearValues();
                prefixRecord.setValue("dxcc", dxcc_id);
                prefixRecord.setValue("exact", !prefixFormat.cap(1).isEmpty());
                prefixRecord.setValue("prefix", prefixFormat.cap(2));
                prefixRecord.setValue("cqz", prefixFormat.cap(3).toInt());
                prefixRecord.setValue("ituz", prefixFormat.cap(4).toInt());

                prefixTableModel.insertRecord(-1, prefixRecord);
            }
            else  {
                qDebug() << "Failed to match " << prefix;
            }
        }

        prefixTableModel.submitAll();

        emit progress(count);
        QCoreApplication::processEvents();

        count++;
    }

    qDebug() << "DXCC update finished:" << count << "entities loaded.";
}

Cty::~Cty() {

}
