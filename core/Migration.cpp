#include <QProgressDialog>
#include <QtSql>
#include <QDebug>
#include "core/Migration.h"

/**
 * Migrate the database to the latest schema version.
 * Returns true on success.
 */
bool Migration::run() {
    int currentVersion = getVersion();

    if (currentVersion == latestVersion) {
        qDebug() << "Database already up to date";
        return true;
    }
    else if (currentVersion < latestVersion) {
        qDebug() << "Starting database migration";
    }
    else {
        qCritical() << "database from the future";
        return false;
    }

    while (true) {
        currentVersion = getVersion();

        if (currentVersion < latestVersion) {
            bool res = migrate(currentVersion+1);
            if (!res || getVersion() == currentVersion) {
                qCritical() << "migration failed";
                return false;
            }
        }
        else break;
    }

    updateBands();
    updateModes();

    qDebug() << "Database migration successful";

    return true;
}

/**
 * Returns the current user_version of the database.
 */
int Migration::getVersion() {
    QSqlQuery query("SELECT version FROM schema_versions "
                    "ORDER BY version DESC LIMIT 1");

    if (query.first()) {
        return query.value(0).toInt();
    }
    else {
        return 0;
    }
}

/**
 * Changes the user_version of the database to version.
 * Returns true of the operation was successful.
 */
bool Migration::setVersion(int version) {
    QSqlQuery query;
    query.prepare("INSERT INTO schema_versions (version, updated)"
                  "VALUES (:version, timezone('utc', now()))");

    query.bindValue(":version", version);

    if (!query.exec()) {
        qWarning() << "setting schema version failed" << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

/**
 * Migrate the database to the given version.
 * Returns true if the operation was successful.
 */
bool Migration::migrate(int toVersion) {
    qDebug() << "migrate to" << toVersion;

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qCritical() << "transaction failed";
        return false;
    }

    bool result = false;
    switch (toVersion) {
    case 1: result = migrate1(); break;
    case 2: result = migrate2(); break;
    case 3: result = migrate3(); break;
    }

    if (result && setVersion(toVersion) && db.commit()) {
        return true;
    }
    else {
        if (!db.rollback()) {
            qCritical() << "rollback failed";
        }
        return false;
    }
}

bool Migration::migrate1() {
    QSqlQuery query;
    bool result = true;

    result &= query.exec("CREATE TABLE IF NOT EXISTS schema_versions"
                         "(version SERIAL PRIMARY KEY,"
                         "updated TIMESTAMP NOT NULL)");

    result &= query.exec("CREATE TABLE IF NOT EXISTS contacts\n"
                         "(id SERIAL PRIMARY KEY,\n"
                         "start_time TIMESTAMP WITH TIME ZONE,\n"
                         "end_time TIMESTAMP WITH TIME ZONE,\n"
                         "callsign VARCHAR(25) NOT NULL,\n"
                         "rst_sent VARCHAR(10),"
                         "rst_rcvd VARCHAR(10),\n"
                         "freq DECIMAL(12,6),\n"
                         "band VARCHAR(10),\n"
                         "mode VARCHAR(15),\n"
                         "submode VARCHAR(15),\n"
                         "name VARCHAR(50),\n"
                         "qth VARCHAR(50),\n"
                         "gridsquare VARCHAR(12),\n"
                         "dxcc INTEGER,\n"
                         "country VARCHAR(50),\n"
                         "cont VARCHAR(2),\n"
                         "cqz INTEGER,\n"
                         "ituz INTEGER,\n"
                         "pfx VARCHAR(10),\n"
                         "fields JSON"
                         ")"
                         );

    return result;
}

bool Migration::migrate2() {
    QSqlQuery query;
    bool result = true;

    result &= query.exec("CREATE TABLE IF NOT EXISTS bands\n"
                         "(id SERIAL PRIMARY KEY,\n"
                         "name VARCHAR(10) UNIQUE NOT NULL,"
                         "start_freq DECIMAL(12,6),"
                         "end_freq DECIMAL(12,6),"
                         "enabled BOOLEAN"
                         ")"
                         );

    result &= query.exec("CREATE TYPE dxcc_mode AS ENUM ('CW', 'PHONE', 'DIGITAL')");

    result &= query.exec("CREATE TABLE IF NOT EXISTS modes\n" "(id SERIAL PRIMARY KEY,\n"
                         "name VARCHAR(15) UNIQUE NOT NULL,"
                         "submodes JSON,"
                         "rprt VARCHAR(10),"
                         "dxcc dxcc_mode,"
                         "enabled BOOLEAN"
                         ")"
                         );

    return result;
}

bool Migration::migrate3() {
    QSqlQuery query;
    bool result = true;

    result &= query.exec("CREATE TABLE IF NOT EXISTS dxcc_entities\n"
                         "(id SERIAL PRIMARY KEY,\n"
                         "name VARCHAR(50) NOT NULL,"
                         "prefix VARCHAR(10),"
                         "cont VARCHAR(2),"
                         "cqz INTEGER,"
                         "ituz INTEGER,"
                         "lat FLOAT,"
                         "lon FLOAT,"
                         "tz FLOAT"
                         ")"
                         );

    result &= query.exec("CREATE TABLE IF NOT EXISTS dxcc_prefixes\n"
                         "(id SERIAL PRIMARY KEY,\n"
                         "prefix VARCHAR(25) UNIQUE NOT NULL,"
                         "exact BOOLEAN,"
                         "dxcc INTEGER REFERENCES dxcc_entities(id),"
                         "cqz INTEGER,"
                         "ituz INTEGER,"
                         "cont VARCHAR(2),"
                         "lat FLOAT,"
                         "lon FLOAT"
                         ")"
                         );

    return result;
}


bool Migration::updateBands() {
    QFile file(":/res/data/bands.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    QSqlTableModel model;
    model.setTable("bands");
    model.removeColumn(model.fieldIndex("id"));
    QSqlRecord record = model.record();

    model.select();

    model.removeRows(0, model.rowCount());
    model.submitAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap bandData = object.toMap();

        record = model.record();

        record.clearValues();

        record.setValue("name", bandData.value("name").toString());
        record.setValue("start_freq", bandData.value("start").toFloat());
        record.setValue("end_freq", bandData.value("end").toFloat());
        record.setValue("enabled", true);

        model.insertRecord(-1, record);
    }

    model.submitAll();
    return true;
}

bool Migration::updateModes() {
    QFile file(":/res/data/modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    QSqlTableModel model;
    model.setTable("modes");
    model.removeColumn(model.fieldIndex("id"));
    QSqlRecord record = model.record();

    model.select();

    model.removeRows(0, model.rowCount());
    model.submitAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap modeData = object.toMap();

        record = model.record();

        record.clearValues();

        QJsonDocument submodes = QJsonDocument(modeData.value("submodes").toJsonArray());

        record.setValue("name", modeData.value("name").toString());
        record.setValue("rprt", modeData.value("rprt").toString());
        record.setValue("dxcc", modeData.value("dxcc").toString());
        record.setValue("submodes", QString(submodes.toJson(QJsonDocument::Compact)));
        record.setValue("enabled", true);

        model.insertRecord(-1, record);
    }

    model.submitAll();

    qDebug() << model.lastError();

    return true;

}

