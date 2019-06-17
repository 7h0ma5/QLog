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
        qDebug() << "database already up to date";
        return true;
    }
    else {
        qCritical() << "database from the future";
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

    qDebug() << "database migration successful";

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
    bool result = true;

    QSqlQuery query;

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

    qDebug() << query.lastError();

    return result;
}
