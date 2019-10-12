#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QColor>
#include "Data.h"

Data::Data(QObject *parent) : QObject(parent) {
    loadContests();
    loadPropagationModes();
    loadLegacyModes();
    loadDxccFlags();
}

Data* Data::instance() {
    static Data instance;
    return &instance;
}

DxccStatus Data::dxccStatus(int dxcc, QString band, QString mode) {
    QString filter;

    QSettings settings;
    QVariant start = settings.value("dxcc/start");
    if (!start.isNull()) {
        filter = QString("AND start_time >= '%1'").arg(start.toDate().toString("yyyy-MM-dd"));
    }

    QString sql_mode = ":mode ";
    if (mode != "CW" && mode != "PHONE" && mode != "DIGITAL") {
        sql_mode = "(SELECT modes.dxcc FROM modes WHERE modes.name = :mode LIMIT 1) ";
    }

    QSqlQuery query;
    query.prepare("SELECT (SELECT contacts.callsign FROM contacts WHERE dxcc = :dxcc " + filter + " ORDER BY start_time ASC LIMIT 1) as entity,"
                  "(SELECT contacts.callsign FROM contacts WHERE dxcc = :dxcc AND band = :band " + filter + " ORDER BY start_time ASC LIMIT 1) as band,"
                  "(SELECT contacts.callsign FROM contacts INNER JOIN modes ON (modes.name = contacts.mode)"
                  "        WHERE contacts.dxcc = :dxcc AND modes.dxcc = " + sql_mode + filter +
                  "        ORDER BY start_time ASC LIMIT 1) as mode,"
                  "(SELECT contacts.callsign FROM contacts INNER JOIN modes ON (modes.name = contacts.mode)"
                  "        WHERE contacts.dxcc = :dxcc AND modes.dxcc = " + sql_mode + filter +
                  "        AND band = :band ORDER BY start_time ASC LIMIT 1) as slot;");

    query.bindValue(":dxcc", dxcc);
    query.bindValue(":band", band);
    query.bindValue(":mode", mode);
    query.exec();

    if (query.next()) {
        if (query.value(0).isNull()) {
            return DxccStatus::NewEntity;
        }
        if (query.value(1).isNull()) {
            if (query.value(2).isNull()) {
                return DxccStatus::NewBandMode;
            }
            else {
                return DxccStatus::NewBand;
            }
        }
        if (query.value(2).isNull()) {
            return DxccStatus::NewMode;
        }
        if (query.value(3).isNull()) {
            return DxccStatus::NewSlot;
        }
        else {
            return DxccStatus::Worked;
        }
    }
    else {
        return DxccStatus::Unknown;
    }
}

Band Data::band(double freq) {
    QSqlQuery query;
    query.prepare("SELECT name, start_freq, end_freq FROM bands WHERE :freq BETWEEN start_freq AND end_freq");
    query.bindValue(0, freq);
    query.exec();

    if (query.next()) {
        Band band;
        band.name = query.value(0).toString();
        band.start = query.value(1).toDouble();
        band.end = query.value(2).toDouble();
        return band;
    }
    else {
        return Band();
    }
}

QString Data::freqToMode(double freq) {
    // 2200m
    if (freq >= 0.1357 && freq <= 0.1378) return "CW";
    // 630m
    else if (freq >= 0.472 && freq <= 0.475) return "CW";
    else if (freq <= 0.479) return "DIGITAL";
    // 160m
    else if (freq >= 1.800 && freq <= 1.840) return "CW";
    else if (freq <= 1.843) return "DIGITAL";
    else if (freq <= 2.000) return "PHONE";
    // 80m
    else if (freq >= 3.500 && freq <= 3.570) return "CW";
    else if (freq <= 3.600) return "DIGITAL";
    else if (freq <= 4.000) return "PHONE";
    // 60m
    else if (freq >= 5.3515 && freq <= 5.354) return "CW";
    else if (freq <= 5.366) return "PHONE";
    // 40m
    else if (freq >= 7.000 && freq <= 7.040) return "CW";
    else if (freq <= 7.050) return "DIGITAL";
    else if (freq <= 7.300) return "PHONE";
    // 30m
    else if (freq >= 10.100 && freq <= 10.130) return "CW";
    else if (freq <= 10.150) return "DIGITAL";
    // 20m
    else if (freq >= 14.000 && freq <= 14.070) return "CW";
    else if (freq <= 14.100) return "DIGITAL";
    else if (freq <= 14.350) return "PHONE";
    // 17m
    else if (freq >= 18.068 && freq <= 18.095) return "CW";
    else if (freq <= 18.111) return "DIGITAL";
    else if (freq <= 18.168) return "PHONE";
    // 15m
    else if (freq >= 21.000 && freq <= 21.070) return "CW";
    else if (freq <= 21.150) return "DIGITAL";
    else if (freq <= 21.450) return "PHONE";
    // 12m
    else if (freq >= 24.000 && freq <= 24.915) return "CW";
    else if (freq <= 24.931) return "DIGITAL";
    else if (freq <= 24.990) return "PHONE";
    // 10m
    else if (freq >= 28.000 && freq <= 28.070) return "CW";
    else if (freq <= 28.225) return "DIGITAL";
    else if (freq <= 29.700) return "PHONE";
    else return QString();
}

QColor Data::statusToColor(DxccStatus status, QColor defaultColor) {
    switch (status) {
        case DxccStatus::NewEntity:
            return QColor(229, 57, 53);
        case DxccStatus::NewBand:
        case DxccStatus::NewMode:
        case DxccStatus::NewBandMode:
            return QColor(76, 175, 80);
        case DxccStatus::NewSlot:
            return QColor(30, 136, 229);
        default:
            return defaultColor;
    }
}


QColor Data::statusToInverseColor(DxccStatus status, QColor defaultColor) {
    switch (status) {
        case DxccStatus::NewEntity:
            return QColor(Qt::white);
        case DxccStatus::NewBand:
        case DxccStatus::NewMode:
        case DxccStatus::NewBandMode:
            return QColor(Qt::white);
        case DxccStatus::NewSlot:
            return QColor(Qt::black);
        default:
            return defaultColor;
    }
}

QPair<QString, QString> Data::legacyMode(QString mode) {
    return legacyModes.value(mode);
}

void Data::loadContests() {
    QFile file(":/res/data/contests.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap contestData = object.toMap();

        QString id = contestData.value("id").toString();
        QString name = contestData.value("name").toString();

        contests.insert(id, name);
    }
}

void Data::loadPropagationModes() {
    QFile file(":/res/data/propagation_modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap propagationModeData = object.toMap();

        QString id = propagationModeData.value("id").toString();
        QString name = propagationModeData.value("name").toString();

        propagationModes.insert(id, name);
    }
}

void Data::loadLegacyModes() {
    QFile file(":/res/data/legacy_modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    QVariantMap modes = QJsonDocument::fromJson(data).toVariant().toMap();

    for (QString key : modes.keys()) {
        QVariantMap legacyModeData = modes[key].toMap();

        QString mode = legacyModeData.value("mode").toString();
        QString submode = legacyModeData.value("submode").toString();
        QPair<QString, QString> modes = QPair<QString, QString>(mode, submode);

        legacyModes.insert(key, modes);
    }
}

void Data::loadDxccFlags() {
    QFile file(":/res/data/dxcc.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap dxccData = object.toMap();

        int id = dxccData.value("id").toInt();
        QString flag = dxccData.value("flag").toString();

        flags.insert(id, flag);
    }
}

DxccEntity Data::lookupDxcc(QString callsign) {
    QSqlQuery query;
    query.prepare(
                "SELECT\n"
                "    dxcc_entities.id,\n"
                "    dxcc_entities.name,\n"
                "    dxcc_entities.prefix,\n"
                "    dxcc_entities.cont,\n"
                "    CASE\n"
                "        WHEN (dxcc_prefixes.cqz != 0)\n"
                "        THEN dxcc_prefixes.cqz\n"
                "        ELSE dxcc_entities.cqz\n"
                "    END AS cqz,\n"
                "    CASE\n"
                "        WHEN (dxcc_prefixes.ituz != 0)\n"
                "        THEN dxcc_prefixes.ituz\n"
                "        ELSE dxcc_entities.ituz\n"
                "    END AS ituz\n,"
                "    dxcc_entities.lat,\n"
                "    dxcc_entities.lon,\n"
                "    dxcc_entities.tz\n"
                "FROM dxcc_prefixes\n"
                "INNER JOIN dxcc_entities ON (dxcc_prefixes.dxcc = dxcc_entities.id)\n"
                "WHERE (dxcc_prefixes.prefix = :callsign and dxcc_prefixes.exact = true)\n"
                "    OR (dxcc_prefixes.exact = false and :callsign LIKE dxcc_prefixes.prefix || '%')\n"
                "ORDER BY dxcc_prefixes.prefix\n"
                "DESC LIMIT 1\n"
    );

    query.bindValue(":callsign", callsign);
    query.exec();

    DxccEntity dxcc;
    if (query.next()) {
        dxcc.dxcc = query.value(0).toInt();
        dxcc.country = query.value(1).toString();
        dxcc.prefix = query.value(2).toString();
        dxcc.cont = query.value(3).toString();
        dxcc.cqz = query.value(4).toInt();
        dxcc.ituz = query.value(5).toInt();
        dxcc.latlon[0] = query.value(6).toDouble();
        dxcc.latlon[1] = query.value(7).toDouble();
        dxcc.tz = query.value(8).toFloat();
        dxcc.flag = flags.value(dxcc.dxcc);
    }
    else {
        dxcc.dxcc = 0;
    }
    return dxcc;
}

QString Data::dxccFlag(int dxcc) {
    return flags.value(dxcc);
}
