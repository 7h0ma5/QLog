#include <QJsonDocument>
#include "core/Cty.h"
#include "Data.h"

Data::Data(QObject *parent) : QObject(parent) {
    cty = new Cty();
    loadBands();
    loadModes();
    loadContests();
    loadPropagationModes();
}

Data* Data::instance() {
    static Data instance;
    return &instance;
}

Band Data::band(float freq) {
    qDebug() << freq;
    for (Band band : bands.values()) {
        qDebug() << band.name() << band.start() << band.end();
        if (freq > band.start() && freq < band.end()) {
            return band;
        }
    }
    return Band();
}

void Data::loadBands() {
    QFile file(":/res/data/bands.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap bandData = object.toMap();

        Band band(
            bandData.value("name").toString(),
            bandData.value("start").toFloat(),
            bandData.value("end").toFloat()
        );

        bands.insert(band.name(), band);
    }
}

void Data::loadModes() {
    QFile file(":/res/data/modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    for (QVariant object : QJsonDocument::fromJson(data).toVariant().toList()) {
        QVariantMap modeData = object.toMap();

        Mode mode(modeData.value("name").toString());

        mode.setDefaultRst(modeData.value("rst").toString());
        mode.addSubmodes(modeData.value("submodes").toStringList());

        modes.insert(mode.name(), mode);
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
