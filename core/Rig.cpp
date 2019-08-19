#include <QDebug>
#include <QSettings>
#include <cstring>
#include <hamlib/rig.h>
#include "Rig.h"

Rig* Rig::instance() {
    static Rig instance;
    return &instance;
}

void Rig::start() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void Rig::update() {
    if (!rig) return;

    if (!rigLock.tryLock(200)) return;

    freq_t rigFreq;
    rig_get_freq(rig, RIG_VFO_CURR, &rigFreq);

    if (rigFreq != freq) {
        freq = rigFreq;
        emit frequencyChanged(freq/1e6);
    }

    rmode_t modeId;
    pbwidth_t pbwidth;
    rig_get_mode(rig, RIG_VFO_CURR, &modeId, &pbwidth);

    QString rigMode;
    switch (modeId) {
    case RIG_MODE_AM:
        rigMode = "AM";
        break;
    case RIG_MODE_CW:
        rigMode = "CW";
        break;
    case RIG_MODE_USB:
    case RIG_MODE_LSB:
        rigMode = "SSB";
        break;
    case RIG_MODE_FM:
        rigMode = "FM";
        break;
    default:
        rigMode = "";
    }

    if (rigMode != mode) {
        mode = rigMode;
        emit modeChanged(mode);
    }

    value_t rigPowerLevel;
    unsigned int rigPower;
    rig_get_level(rig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &rigPowerLevel);
    rig_power2mW(rig, &rigPower, rigPowerLevel.f, rigFreq, modeId);

    qDebug() << "power" << rigPowerLevel.f;

    if (rigPower != power) {
        power = rigPower;
        emit powerChanged(power/1000.0);
    }

    rigLock.unlock();
}

void Rig::open() {
    QSettings settings;
    int model = settings.value("hamlib/rig/model").toInt();
    int baudrate = settings.value("hamlib/rig/baudrate").toInt();
    QByteArray portStr = settings.value("hamlib/rig/port").toByteArray();
    const char* port = portStr.constData();

    qDebug() << portStr;

    rigLock.lock();

    rig = rig_init(model);

    strncpy(rig->state.rigport.pathname, port, FILPATHLEN - 1);
    rig->state.rigport.parm.serial.rate = baudrate;

    int status = rig_open(rig);

    rigLock.unlock();

    if (status != RIG_OK) {
        qWarning() << "rig connection error";
    }
    else {
        qDebug() << "connected to rig";
    }
}

void Rig::setFrequency(double newFreq) {
    if (!rig) return;
    freq = newFreq*1e6;

    rigLock.lock();
    rig_set_freq(rig, RIG_VFO_CURR, freq);
    rigLock.unlock();
}

void Rig::setMode(QString newMode) {
    if (!rig) return;
    mode = newMode;
    rigLock.lock();
    if (newMode == "CW") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_CW, RIG_PASSBAND_NORMAL);
    }
    else if (newMode == "SSB") {
        if (freq < 10) {
            rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_LSB, RIG_PASSBAND_NORMAL);
        }
        else {
            rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_USB, RIG_PASSBAND_NORMAL);
        }
    }
    else if (newMode == "AM") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_AM, RIG_PASSBAND_NORMAL);
    }
    else if (newMode == "FM") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_FM, RIG_PASSBAND_NORMAL);
    }
    rigLock.unlock();
}

void Rig::setPower(double newPower) {
    if (!rig) return;
    power = (int)(newPower*1000);
}
