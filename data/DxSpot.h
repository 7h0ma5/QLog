#ifndef DXSPOT_H
#define DXSPOT_H

#include <QtCore>
#include "Dxcc.h"

class DxSpot {
public:
    QTime time;
    QString callsign;
    double freq;
    QString band;
    QString spotter;
    QString comment;
    DxccEntity dxcc;
    DxccStatus status;
};


#endif // DXSPOT_H
