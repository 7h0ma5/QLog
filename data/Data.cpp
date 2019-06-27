#include <QJsonDocument>
#include <QSqlQuery>
#include "core/Cty.h"
#include "Data.h"

Data::Data(QObject *parent) : QObject(parent) {
    cty = new Cty();
    loadContests();
    loadPropagationModes();
}

Data* Data::instance() {
    static Data instance;
    return &instance;
}

QString Data::band(double freq) {
    QSqlQuery query;
    query.prepare("SELECT name FROM bands WHERE :freq BETWEEN start_freq AND end_freq");
    query.bindValue(0, freq);
    query.exec();

    if (query.next()) {
        return query.value(0).toString();
    }
    else {
        return QString();
    }
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

DxccEntity Data::lookupDxcc(QString callsign) {
    return cty->lookup(callsign);
}
