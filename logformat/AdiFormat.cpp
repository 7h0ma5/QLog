#include <QSqlRecord>
#include <QDebug>
#include "data/Data.h"
#include "AdiFormat.h"

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

    QString typeString;
    QString lengthString;
    int length = 0;

    while (!stream.atEnd()) {
        switch (state) {
        case START:
            stream >> c;
            if (c == '<') {
                inHeader = false;
                state = KEY;
                field = "";
            }
            else {
                inHeader = true;
                state = FIELD;
            }
            break;

        case FIELD:
            stream >> c;
            if (c == '<') {
                state = KEY;
                field = "";
            }
            break;

        case KEY:
            stream >> c;
            if (c == ':') {
                state = SIZE;
                lengthString = "";
            }
            else if (c == '>') {
                state = FIELD;
                if (inHeader && field.toLower() == "eoh") {
                    inHeader = false;
                }
                else {
                    value = "";
                    return;
                }
            }
            else {
                field.append(c);
            }
            break;

        case SIZE:
            stream >> c;
            if (c == ':') {
                 if (!lengthString.isEmpty()) {
                    length = lengthString.toInt();
                 }
                 state = DATA_TYPE;
                 typeString = "";
            }
            else if (c == '>') {
                if (!lengthString.isEmpty()) {
                    length = lengthString.toInt();
                }

                if (length > 0) {
                    state = VALUE;
                    value = "";
                }
                else {
                    state = FIELD;
                    if (!inHeader) {
                        value = "";
                        return;
                    }

                }
            }
            else {
                lengthString.append(c);
            }
            break;

        case DATA_TYPE:
            stream >> c;
            if (c == '>') {
                if (length > 0) {
                    state = VALUE;
                    value = "";
                }
                else {
                    state = FIELD;
                    if (!inHeader) {
                        value = "";
                        return;
                    }
                }
            }
            else {
                typeString.append(c);
            }
            break;

        case VALUE:
            value = QString(stream.read(length));
            state = FIELD;
            if (!inHeader) {
                return;
            }
            break;
        }
    }
}

bool AdiFormat::readContact(QMap<QString, QVariant>& contact) {
    QString field;
    QString value;

    while (!stream.atEnd()) {
        readField(field, value);
        field = field.toLower();

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
    record.setValue("gridsquare", contact.take("gridsquare").toString().toUpper());
    record.setValue("cqz", contact.take("cqz"));
    record.setValue("ituz", contact.take("ituz"));
    record.setValue("freq", contact.take("freq"));
    record.setValue("band", contact.take("band").toString().toLower());
    record.setValue("cont", contact.take("cont").toString().toUpper());
    record.setValue("dxcc", contact.take("dxcc"));
    record.setValue("country", contact.take("country"));
    record.setValue("pfx", contact.take("pfx").toString().toUpper());
    record.setValue("state", contact.take("state").toString().toUpper());
    record.setValue("cnty", contact.take("cnty"));
    record.setValue("iota", contact.take("iota").toString().toUpper());
    record.setValue("qsl_rcvd", contact.take("qsl_rcvd"));
    record.setValue("qslrdate", parseDate(contact.take("qslrdate").toString()));
    record.setValue("qsl_sent", contact.take("qsl_sent"));
    record.setValue("qslsdate", parseDate(contact.take("qslsdate").toString()));
    record.setValue("lotw_qsl_rcvd", contact.take("lotw_qsl_rcvd"));
    record.setValue("lotw_qslrdate", parseDate(contact.take("lotw_qslrdate").toString()));
    record.setValue("lotw_qsl_sent", contact.take("lotw_qsl_sent"));
    record.setValue("lotw_qslsdate", parseDate(contact.take("lotw_qslsdate").toString()));
    record.setValue("tx_pwr", contact.take("tx_pwr").toDouble());

    QString mode = contact.take("mode").toString().toUpper();
    QString submode = contact.take("submode").toString().toUpper();

    QPair<QString, QString> legacy = Data::instance()->legacyMode(mode);
    if (!legacy.first.isEmpty()) {
        mode = legacy.first;
        submode = legacy.second;
    }

    record.setValue("mode", mode);
    record.setValue("submode", submode);

    QDate date_on = parseDate(contact.take("qso_date").toString());
    QDate date_off = parseDate(contact.take("qso_date_off").toString());

    if (date_off.isNull() || !date_off.isValid()) {
        date_off = date_on;
    }

    QTime time_on = parseTime(contact.take("time_on").toString());
    QTime time_off = parseTime(contact.take("time_off").toString());

    if (time_on.isValid() && time_off.isNull()) {
        time_off = time_on;
    }
    if (time_off.isValid() && time_on.isNull()) {
        time_on = time_off;
    }

    QDateTime start_time(date_on, time_on, Qt::UTC);
    QDateTime end_time(date_off, time_off, Qt::UTC);

    if (end_time < start_time) {
        qDebug() << "End time before start time!" << record;
    }

    record.setValue("start_time", start_time);
    record.setValue("end_time", end_time);

    if (defaults) {
        foreach (QString key, defaults->keys()) {
            if (contact.value(key).isNull()) {
                contact.insert(key, defaults->value(key));
            }
        }
    }

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(contact));
    record.setValue("fields", QString(doc.toJson()));

    return true;
}

QDate AdiFormat::parseDate(QString date) {
    if (date.length() == 8) {
        return QDate::fromString(date, "yyyyMMdd");
    }
    else {
        return QDate();
    }
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
