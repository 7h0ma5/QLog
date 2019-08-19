#include <hamlib/rotator.h>
#include "Rotator.h"

Rotator::Rotator() : QObject(nullptr)
{

}

Rotator* Rotator::instance() {
    static Rotator instance;
    return &instance;
}

void Rotator::start() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void Rotator::update() {
    if (!rot) return;

    if (!rotLock.tryLock(200)) return;

    azimuth_t az;
    elevation_t el;

    rot_get_position(rot, &az, &el);

    int newAzimuth = static_cast<int>(az);
    int newElevation = static_cast<int>(el);

    if (newAzimuth != this->azimuth || newElevation != this->elevation)  {
        this->azimuth = newAzimuth;
        this->elevation = newElevation;
        emit positionChanged(azimuth, elevation);
    }
    rotLock.unlock();
}

void Rotator::open() {
    QSettings settings;
    int model = settings.value("hamlib/rot/model").toInt();
    int baudrate = settings.value("hamlib/rot/baudrate").toInt();
    QByteArray portStr = settings.value("hamlib/rot/port").toByteArray();
    const char* port = portStr.constData();

    qDebug() << portStr;

    rotLock.lock();

    rot = rot_init(model);

    strncpy(rot->state.rotport.pathname, port, FILPATHLEN - 1);
    rot->state.rotport.parm.serial.rate = baudrate;

    int status = rot_open(rot);

    rotLock.unlock();

    if (status != RIG_OK) {
        qWarning() << "rotator connection error";
    }
    else {
        qDebug() << "connected to rotator";
    }
}

void Rotator::setPosition(int azimuth, int elevation) {
    if (!rot) return;
    rotLock.lock();
    rot_set_position(rot, static_cast<azimuth_t>(azimuth), static_cast<elevation_t>(elevation));
    rotLock.unlock();
}
