#include <QtSql>
#include "LogFormat.h"
#include "AdiFormat.h"
#include "AdxFormat.h"
#include "JsonFormat.h"
#include "core/utils.h"
#include "data/Data.h"

LogFormat::LogFormat(QTextStream& stream) : QObject(nullptr), stream(stream) {
    this->defaults = nullptr;
}

LogFormat::~LogFormat() {

}

LogFormat* LogFormat::open(QString type, QTextStream& stream) {
    type = type.toLower();

    if (type == "adi") {
        return open(LogFormat::ADI, stream);
    }
    else if (type == "adx") {
        return open(LogFormat::ADX, stream);
    }
    else if (type == "json") {
        return open(LogFormat::JSON, stream);
    }
    else if (type == "cabrillo") {
        return open(LogFormat::JSON, stream);
    }
    else {
        return nullptr;
    }
}

LogFormat* LogFormat::open(LogFormat::Type type, QTextStream& stream) {
    switch (type) {
    case LogFormat::ADI:
        return new AdiFormat(stream);

    case LogFormat::ADX:
        return new AdxFormat(stream);

    case LogFormat::JSON:
        return new JsonFormat(stream);

    case LogFormat::CABRILLO:
        return nullptr;

    default:
        return nullptr;
    }
}

void LogFormat::setDefaults(QMap<QString, QString>& defaults) {
    this->defaults = &defaults;
}

void LogFormat::setDateRange(QDate start, QDate end) {
    this->startDate = start;
    this->endDate = end;
}

void LogFormat::setUpdateDxcc(bool updateDxcc) {
    this->updateDxcc = updateDxcc;
}

void LogFormat::runImport() {
    this->importStart();

    int count = 0;

    QSqlTableModel model;
    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));
    QSqlRecord record = model.record();

    while (true) {
        record.clearValues();

        if (!this->importNext(record)) break;

        if (dateRangeSet()) {
            if (!inDateRange(record.value("start_time").toDateTime().date())) {
                continue;
            }
        }

        DxccEntity entity = Data::instance()->lookupDxcc(record.value("callsign").toString());

        if ((record.value("dxcc").isNull() || updateDxcc) && entity.dxcc) {
            record.setValue("dxcc", entity.dxcc);
            record.setValue("country", entity.country);
        }

        if (record.value("cont").isNull() && entity.dxcc) {
            record.setValue("cont", entity.cont);
        }

        if (record.value("ituz").isNull() && entity.dxcc) {
            record.setValue("ituz", QString::number(entity.ituz));
        }

        if (record.value("cqz").isNull() && entity.dxcc) {
            record.setValue("cqz", QString::number(entity.cqz));
        }

        if (record.value("band").isNull() && !record.value("frequency").isNull()) {
            double freq = record.value("frequency").toDouble();
            record.setValue("band", freqToBand(freq));
        }

        model.insertRecord(-1, record);

        if (count % 10 == 0) {
            emit progress(stream.pos());
        }

        count++;
    }

    model.submitAll();

    this->importEnd();

    emit finished(count);
}

int LogFormat::runExport() {
    this->exportStart();

    QSqlQuery query;
    if (dateRangeSet()) {
        query.prepare("SELECT * FROM contacts"
                      " WHERE (start_time BETWEEN :start_date AND :end_date)"
                      " ORDER BY start_time ASC");
        query.bindValue(":start_date", QDateTime(startDate));
        query.bindValue(":end_date", QDateTime(endDate));
    }
    else {
        query.prepare("SELECT * FROM contacts ORDER BY start_time ASC");
    }

    query.exec();

    int count = 0;
    while (query.next()) {
        QSqlRecord record = query.record();
        this->exportContact(record);
        count++;
    }

    this->exportEnd();
    return count;
}

bool LogFormat::dateRangeSet() {
    return !startDate.isNull() && !endDate.isNull();
}

bool LogFormat::inDateRange(QDate date) {
    return date >= startDate && date <= endDate;
}

