#ifndef DXCC_H
#define DXCC_H

#include <QtCore>

enum DxccStatus {
    NewEntity = 0b1,
    NewBandMode = 0b110,
    NewBand = 0b010,
    NewMode = 0b100,
    NewSlot = 0b1000,
    Worked = 0b10000,
    Unknown = 0b100000
};

class DxccEntity {
public:
    QString country;
    QString prefix;
    qint32 dxcc;
    QString cont;
    qint32 cqz;
    qint32 ituz;
    double latlon[2];
    float tz;
    QString flag;
};

struct DxccPrefix {
public:
    QString prefix;
    bool exact;
    qint32 dxcc;
    qint32 cqz;
    qint32 ituz;
    QString cont;
    double latlon[2];
};

#endif // DXCC_H
