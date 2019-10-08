#include <QSqlRecord>
#include <QtXml>
#include "logformat/AdxFormat.h"

void AdxFormat::exportStart() {
    QString date = QDateTime::currentDateTimeUtc().toString("yyyyMMdd hhmmss");

    writer = new QXmlStreamWriter(stream.device());
    writer->setAutoFormatting(true);

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

    QDateTime time_start = record.value("start_time").toDateTime().toTimeSpec(Qt::UTC);
    QDateTime time_end = record.value("end_time").toDateTime().toTimeSpec(Qt::UTC);

    writeField("call", record.value("callsign").toString());
    writeField("qso_date", time_start.toString("yyyyMMdd"));
    writeField("time_on", time_start.toString("hhmmss"));
    writeField("qso_date_off", time_end.toString("yyyyMMdd"));
    writeField("time_off", time_end.toString("hhmmss"));
    writeField("rst_rcvd", record.value("rst_rcvd").toString());
    writeField("rst_sent", record.value("rst_sent").toString());
    writeField("name", record.value("name").toString());
    writeField("qth", record.value("qth").toString());
    writeField("gridsquare", record.value("gridsquare").toString());
    writeField("cqz", record.value("cqz").toString());
    writeField("ituz", record.value("ituz").toString());
    writeField("freq", record.value("freq").toString());
    writeField("band", record.value("band").toString());
    writeField("mode", record.value("mode").toString());
    writeField("submode", record.value("submode").toString());
    writeField("cont", record.value("cont").toString());
    writeField("dxcc", record.value("dxcc").toString());
    writeField("country", record.value("country").toString());
    writeField("pfx", record.value("pfx").toString());
    writeField("state", record.value("state").toString());
    writeField("cnty", record.value("cnty").toString());
    writeField("iota", record.value("iota").toString());
    writeField("qsl_rcvd", record.value("qsl_rcvd").toString());
    writeField("qslrdate", record.value("qslrdate").toDate().toString("yyyyMMdd"));
    writeField("qsl_sent", record.value("qsl_sent").toString());
    writeField("qslsdate", record.value("qslsdate").toDate().toString("yyyyMMdd"));
    writeField("lotw_qsl_rcvd", record.value("lotw_qsl_rcvd").toString());
    writeField("lotw_qslrdate", record.value("lotw_qslrdate").toDate().toString("yyyyMMdd"));
    writeField("lotw_qsl_sent", record.value("lotw_qsl_sent").toString());
    writeField("lotw_qslsdate", record.value("lotw_qslsdate").toDate().toString("yyyyMMdd"));
    writeField("tx_pwr", record.value("tx_pwr").toString());

    QJsonObject fields = QJsonDocument::fromJson(record.value("fields").toByteArray()).object();

    for (const QString& key : fields.keys()) {
        writeField(key, fields.value(key).toString());
    }

    writer->writeEndElement();
}

void AdxFormat::writeField(QString name, QString value) {
    if (value.isEmpty()) return;
    writer->writeTextElement(name.toUpper(), value);
}
