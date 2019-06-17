#include <QSqlRecord>
#include <QtXml>
#include "logformat/AdxFormat.h"

void AdxFormat::exportStart() {
    QString date = QDateTime::currentDateTimeUtc().toString("yyyyMMdd hhmmss");

    writer = new QXmlStreamWriter(stream.device());

    writer->writeStartDocument();
    writer->writeStartElement("ADX");

    writer->writeStartElement("HEADER");
    writer->writeTextElement("ADIF_VER", "3.0.4");
    writer->writeTextElement("PROGRAMID", "QLog");
    writer->writeTextElement("PROGRAMVERSION", VERSION);
    writer->writeTextElement("CREATED_TIMESTAMP", date);
    writer->writeEndElement();

    writer->writeStartElement("RECORDS");
}

void AdxFormat::exportEnd() {
    writer->writeEndDocument();
    delete writer;
}

void AdxFormat::exportContact(QSqlRecord& record) {
    writer->writeStartElement("RECORD");

    QDate date = QDate::fromString(record.value("date").toString(), Qt::ISODate);
    QTime time_on = QTime::fromString(record.value("time_on").toString(), Qt::ISODate);
    QTime time_off = QTime::fromString(record.value("time_off").toString(), Qt::ISODate);

    writeField("CALL", record.value("callsign").toString());
    writeField("QSO_DATE", date.toString("yyyyMMdd"));
    writeField("TIME_ON", time_on.toString("hhmmss"));
    writeField("TIME_OFF", time_off.toString("hhmmss"));
    writeField("RST_RCVD", record.value("rst_rcvd").toString());
    writeField("RST_SENT", record.value("rst_sent").toString());
    writeField("NAME", record.value("name").toString());
    writeField("QTH", record.value("qth").toString());
    writeField("GRIDSQUARE", record.value("grid").toString());
    writeField("MY_GRIDSQUARE", record.value("my_grid").toString());
    writeField("CQZ", record.value("cqz").toString());
    writeField("ITUZ", record.value("ituz").toString());
    writeField("FREQ", record.value("frequency").toString());
    writeField("BAND", record.value("band").toString());
    writeField("MODE", record.value("mode").toString());
    writeField("TX_PWR", record.value("tx_power").toString());
    writeField("MY_RIG", record.value("my_rig").toString());
    writeField("COMMENT", record.value("comment").toString());
    writeField("QSL_VIA", record.value("qsl_via").toString());

    writer->writeEndElement();
}

void AdxFormat::writeField(QString name, QString value) {
    if (value.isEmpty()) return;
    writer->writeTextElement(name, value);
}
