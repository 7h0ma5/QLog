#include <QDebug>
#include <QSettings>
#include <cstring>
#include <hamlib/rig.h>
#include "Rig.h"

static QString modeToString(rmode_t mode) {
    switch (mode) {
    case RIG_MODE_AM: return "AM";
    case RIG_MODE_CW: return "CW";
    case RIG_MODE_USB: return "USB";
    case RIG_MODE_LSB: return "LSB";
    case RIG_MODE_FM: return "FM";
    default: return "";
    }
}

Rig* Rig::instance() {
    static Rig instance;
    return &instance;
}

void Rig::start() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(500);
}

void Rig::update() {
    if (!rig) return;

    if (!rigLock.tryLock(200)) return;

    freq_t vfo_freq;

    if (rig_get_freq(rig, RIG_VFO_CURR, &vfo_freq) == RIG_OK) {
        int new_freq = static_cast<int>(vfo_freq);

        if (new_freq != freq_rx) {
            freq_rx = new_freq;
            emit frequencyChanged(freq_rx/1e6);
        }
    }

    rmode_t modeId;
    pbwidth_t pbwidth;

    if (rig_get_mode(rig, RIG_VFO_A, &modeId, &pbwidth) == RIG_OK) {
        QString new_mode = modeToString(modeId);
        if (new_mode != mode_rx)  {
            mode_rx = new_mode;
            emit modeChanged(mode_rx);
        }
    }

    value_t rigPowerLevel;
    unsigned int rigPower;
    rig_get_level(rig, RIG_VFO_CURR, RIG_LEVEL_RFPOWER, &rigPowerLevel);
    rig_power2mW(rig, &rigPower, rigPowerLevel.f, freq_rx, modeId);

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
    return;

    rigLock.lock();
    freq_rx = static_cast<int>(newFreq*1e6);
    rig_set_freq(rig, RIG_VFO_CURR, freq_rx);
    rigLock.unlock();
}

void Rig::setMode(QString newMode) {
    if (!rig) return;
    return;

    rigLock.lock();
    mode_rx = newMode;
    if (newMode == "CW") {
        rig_set_mode(rig, RIG_VFO_CURR, RIG_MODE_CW, RIG_PASSBAND_NORMAL);
    }
    else if (newMode == "SSB") {
        if (freq_rx < 10) {
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
