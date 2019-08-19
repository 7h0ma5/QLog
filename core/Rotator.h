#ifndef ROTATOR_H
#define ROTATOR_H

#include <QtCore>

struct rot;

class Rotator : public QObject
{
    Q_OBJECT

public:
    static Rotator* instance();

signals:
    void positionChanged(int azimuth, int elevation);

public slots:
    void start();
    void update();
    void open();

    void setPosition(int azimuth, int elevation);

private:
    Rotator();

    int azimuth;
    int elevation;

    struct rot* rot;
    QMutex rotLock;
};

#endif // ROTATOR_H
