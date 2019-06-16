#include <QSqlRecord>
#include <QDebug>
#include "adiformat.h"

void AdiFormat::exportStart() {
    stream << "### QLog ADIF Export\n";
    writeField("ADIF_VER", "3.0.4");
    writeField("PROGRAMID", "QLog");
    writeField("PROGRAMVERSION", VERSION);
    writeField("CREATED_TIMESTAMP", QDateTime::currentDateTimeUtc().toString("yyyyMMdd hhmmss"));
    stream << "<EOH>\n\n";
}

void AdiFormat::exportContact(QSqlRecord& record) {
    QDateTime time_start = record.value("start_time").toDateTime().toTimeSpec(Qt::UTC);
    QDateTime time_end = record.value("end_time").toDateTime().toTimeSpec(Qt::UTC);

    writeField("call", record.value("callsign").toString());
    writeField("qso_date", time_start.toString("yyyyMMdd"), "D");
    writeField("time_on", time_start.toString("hhmmss"), "T");
    writeField("qso_date_off", time_end.toString("yyyyMMdd"), "D");
    writeField("time_off", time_end.toString("hhmmss"), "T");
    writeField("rst_rcvd", record.value("rst_rcvd").toString());
    writeField("rst_sent", record.value("rst_sent").toString());
    writeField("name", record.value("name").toString());
    writeField("qth", record.value("qth").toString());
    writeField("gridsquare", record.value("gridsquare").toString());
    writeField("cqz", record.value("cqz").toString());
    writeField("ituz", record.value("ituz").toString());
    writeField("freq", record.value("freq").toString(), "N");
    writeField("band", record.value("band").toString());
    writeField("mode", record.value("mode").toString());
    writeField("submode", record.value("submode").toString());
    writeField("cont", record.value("cont").toString());
    writeField("dxcc", record.value("dxcc").toString());
    writeField("country", record.value("country").toString());
    writeField("pfx", record.value("pfx").toString());

    QJsonObject fields = QJsonDocument::fromJson(record.value("fields").toByteArray()).object();

    for (const QString& key : fields.keys()) {
        writeField(key, fields.value(key).toString());
    }

    stream << "<eor>\n\n";
}

void AdiFormat::writeField(QString name, QString value, QString type) {
    if (value.isEmpty()) return;
    stream << "<" << name << ":" << value.size();
    if (!type.isEmpty()) stream << ":" << type;
    stream << ">" << value << '\n';
    // TODO: handle unicode values
}

void AdiFormat::readField(QString& field, QString& value) {
    char c;

    field = "";
    value = "";

    // find beginning of next field
    while (!stream.atEnd()) {
        stream >> c;
        if (c == '<') break;
    }

    if (stream.atEnd() || c != '<') return;

    // read field name
    while (!stream.atEnd()) {
        stream >> c;
        if (c == ':' || c == '>') break;
        field.append(c);
    }
    field = field.toLower();
    if (c == '>' || stream.atEnd()) return;

    // read field length
    QString lengthString;
    int length = 0;

    while (!stream.atEnd()) {
        stream >> c;
        if (c == ':' || c == '>') break;
        lengthString.append(c);
    }
    if (!lengthString.isEmpty()) {
        length = lengthString.toInt();
    }
    if (stream.atEnd()) {
        return;
    }

     // read field type
    QString type = "";
    if (c == ':') {
        while (!stream.atEnd()) {
            stream >> c;
            if (c == '>') break;
            type.append(c);
        }
    }

    // read field value
    if (c == '>' && length) {
        value = QString(stream.read(length));
    }
}

bool AdiFormat::readContact(QMap<QString, QVariant>& contact) {
    QString field;
    QString value;

    while (!stream.atEnd()) {
        readField(field, value);

        if (field == "eor") {
            return true;
        }

        if (!value.isEmpty()) {
             contact[field] = QVariant(value);
        }
    }

    return false;
}

bool AdiFormat::importNext(QSqlRecord& record) {
    QMap<QString, QVariant> contact;

    if (!readContact(contact)) {
        return false;
    }

    record.setValue("callsign", contact.take("call"));
    record.setValue("rst_rcvd", contact.take("rst_rcvd"));
    record.setValue("rst_sent", contact.take("rst_sent"));
    record.setValue("name", contact.take("name"));
    record.setValue("qth", contact.take("qth"));
    record.setValue("gridsquare", contact.take("gridsquare"));
    record.setValue("cqz", contact.take("cqz"));
    record.setValue("ituz", contact.take("ituz"));
    record.setValue("freq", contact.take("freq"));
    record.setValue("band", contact.take("band"));
    record.setValue("mode", contact.take("mode"));
    record.setValue("submode", contact.take("submode"));
    record.setValue("cont", contact.take("cont"));
    record.setValue("dxcc", contact.take("dxcc"));
    record.setValue("country", contact.take("country"));
    record.setValue("pfx", contact.take("pfx"));

    QDate date = QDate::fromString(contact.take("qso_date").toString(), "yyyyMMdd");

    QTime time_on = parseTime(contact.take("time_on").toString());
    QTime time_off = parseTime(contact.take("time_off").toString());

    if (time_on.isValid() && time_off.isNull()) {
        time_off = time_on;
    }
    if (time_off.isValid() && time_on.isNull()) {
        time_on = time_off;
    }

    QDateTime start_time(date, time_on, Qt::UTC);
    QDateTime end_time(date, time_off, Qt::UTC);

    qDebug() << time_on << start_time;

    record.setValue("start_time", start_time);
    record.setValue("end_time", end_time);

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(contact));
    record.setValue("fields", QString(doc.toJson()));

    return true;
}

QTime AdiFormat::parseTime(QString time) {
    switch (time.length()) {
    case 4:
        return QTime::fromString(time, "hhmm");

    case 6:
        return QTime::fromString(time, "hhmmss");

    default:
        return QTime();
    }
}
