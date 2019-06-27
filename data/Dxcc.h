#ifndef DXCC_H
#define DXCC_H

#include <QtCore>

enum DxccStatus {
    Unknown,
    NewEntity,
    NewBandMode,
    NewBand,
    NewMode,
    NewSlot,
    Worked
};

class DxccEntity {
public:
    QString country;
    QString prefix;
    qint32 dxcc;
    QString cont;
    qint32 cqz;
    qint32 ituz;
    float latlon[2];
    float tz;
};

struct DxccPrefix {
public:
    QString prefix;
    bool exact;
    qint32 dxcc;
    qint32 cqz;
    qint32 ituz;
    QString cont;
    float latlon[2];
};

#endif // DXCC_H
