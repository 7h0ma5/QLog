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
#include "dxcc.h"

#define CTY_URL "http://www.country-files.com/cty/cty.csv"

Dxcc::Dxcc() {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if (dir.exists("cty.csv")) {
        qDebug() << "use cached cty.csv at" << dir.path();
        loadData();
    }
    else {
        download();
    }
}

void Dxcc::loadData() {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile file(dir.filePath("cty.csv"));
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    parseData(stream);
    file.close();
}

void Dxcc::download() {
    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    QUrl url(CTY_URL);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "QLog/1.0 (Qt)");
    nam->get(request);
    qDebug() << "download cty.csv from" << url.toString();
}

DxccEntity Dxcc::lookup(QString callsign) {
    for (int i = callsign.length(); i > 0; i--) {
        QString substring = QStringRef(&callsign, 0, i).toString();

        if (prefixes.contains(substring)) {
            DxccPrefix prefix = prefixes[substring];

            if (prefix.exact && prefix.prefix != callsign) {
                continue;
            }

            DxccEntity result = entities[prefix.dxcc];
            if (prefix.cqz) result.cqz = prefix.cqz;
            if (prefix.ituz) result.ituz = prefix.ituz;

            return result;
        }
    }
    /*
    for (int i = callsign.length(); i > 0; i--) {
        QStringRef substring(&callsign, 0, i);
        if (dxccMap.contains(substring.toString())) {
            return dxccMap[substring.toString()];
        }
    }
    */

    DxccEntity result;
    result.dxcc = 0;
    return result;
}

void Dxcc::processReply(QNetworkReply* reply) {
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

        loadData();
    }
    else {
        qDebug() << "failed to download cty.csv";
    }

    delete reply;
    delete nam;

}

void Dxcc::parseData(QTextStream& data) {
    qDebug() << "parse data";
    QRegExp prefixSeperator("[\\s;]");
    QRegExp prefixFormat("(=?)([A-Z0-9/]+)(?:\\((\\d+)\\))?(?:\\[(\\d+)\\])?$");

    while (!data.atEnd()) {
        QString line = data.readLine();
        QStringList fields = line.split(',');

        if (fields.count() != 10)  {
            qDebug() << "Invalid line in cty.csv";
            continue;
        }

        DxccEntity dxcc;
        dxcc.prefix = fields.at(0);
        dxcc.country = fields.at(1);
        dxcc.dxcc = fields.at(2).toInt();
        dxcc.cont = fields.at(3);
        dxcc.cqz = fields.at(4).toInt();
        dxcc.ituz = fields.at(5).toInt();
        dxcc.latlon[0] = fields.at(6).toFloat();
        dxcc.latlon[1] = -fields.at(7).toFloat();
        dxcc.tz = fields.at(8).toFloat();

        if (entities.contains(dxcc.dxcc)) {
            qDebug() << "ENTITY COLLISION" << dxcc.country;
        }

        entities.insert(dxcc.dxcc, dxcc);

        QStringList prefixList = fields.at(9).split(prefixSeperator, QString::SkipEmptyParts);
        for (QString prefix : prefixList) {
            if (prefixFormat.exactMatch(prefix)) {
                DxccPrefix pfx;
                pfx.dxcc = dxcc.dxcc;
                pfx.exact = !prefixFormat.cap(1).isEmpty();
                pfx.prefix = prefixFormat.cap(2);
                pfx.cqz = prefixFormat.cap(3).toInt();
                pfx.ituz = prefixFormat.cap(4).toInt();

                if (prefixes.contains(pfx.prefix)) {
                    qDebug() << "PREFIX COLLISION" << pfx.prefix;
                }

                prefixes.insert(pfx.prefix, pfx);
            }
            else  {
                qDebug() << "Failed to match " << prefix;
            }
        }
    }
}

Dxcc::~Dxcc() {

}
