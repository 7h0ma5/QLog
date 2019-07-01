#include <QProgressDialog>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>
#include "core/Migration.h"
#include "core/Cty.h"

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

    QProgressDialog progress("Migrating the database...", nullptr, currentVersion, latestVersion);
    progress.show();

    while ((currentVersion = getVersion()) < latestVersion) {
        bool res = migrate(currentVersion+1);
        if (!res || getVersion() == currentVersion) {
            progress.close();
            return false;
        }
        progress.setValue(currentVersion);
    }

    progress.close();

    if (!tableRows("bands")) {
        qDebug() << "Updating band table";
        updateBands();
    }

    if (!tableRows("modes")) {
        qDebug() << "Updating mode table";
        updateModes();
    }

    if (!tableRows("dxcc_entities") || !tableRows("dxcc_prefixes")) {
        updateDxcc();
    }

    qDebug() << "Database migration successful";

    return true;
}

/**
 * Returns the current user_version of the database.
 */
int Migration::getVersion() {
    QSqlQuery query("SELECT version FROM schema_versions "
                    "ORDER BY version DESC LIMIT 1");

    return query.first() ? query.value(0).toInt() : 0;
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

    QString migration_file = QString(":/res/sql/migration_%1.sql").arg(toVersion, 3, 10, QChar('0'));
    bool result = runSqlFile(migration_file);

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

bool Migration::runSqlFile(QString filename) {
    QFile sql_file(filename);
    sql_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString sql = QString(sql_file.readAll());
    QSqlQuery query;
    return query.exec(sql);
}

int Migration::tableRows(QString name) {
    QSqlQuery query(QString("SELECT count(*) FROM %1").arg(name));
    return query.first() ? query.value(0).toInt() : 0;
}

bool Migration::updateBands() {
    return runSqlFile(":/res/sql/bands.sql");
}

bool Migration::updateModes() {
    return runSqlFile(":/res/sql/modes.sql");
}

bool Migration::updateDxcc() {
    QProgressDialog progress("Updating DXCC entities...", nullptr, 0, 346);
    progress.show();

    Cty cty;

    QObject::connect(&cty, &Cty::progress, &progress, &QProgressDialog::setValue);
    QObject::connect(&cty, &Cty::finished, &progress, &QProgressDialog::done);

    cty.update();
    if (!progress.exec()) {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("DXCC update failed."));
        return false;
    }
    return true;
}
