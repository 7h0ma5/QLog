#include <QtCore/QRegExp>
#include <QDebug>
#include <cmath>
#include "utils.h"

#define EARTH_RADIUS 6371
#define EARTH_CIRCUM 40075

bool gridValidate(QString grid) {
    QRegExp regex("^[A-Za-z]{2}[0-9]{2}([A-Za-z]{2})?$");
    return regex.exactMatch(grid);
}

bool gridToCoord(QString grid, double& lat, double& lon) {
    if (!gridValidate(grid)) return false;

    grid = grid.toUpper();

    lon = (grid.at(0).toLatin1() - 'A') * 20 - 180;
    lat = (grid.at(1).toLatin1() - 'A') * 10 - 90;

    lon += (grid.at(2).toLatin1() - '0') * 2;
    lat += (grid.at(3).toLatin1() - '0') * 1;

    if (grid.size() >= 6) {
        lon += (grid.at(4).toLatin1() - 'A') * (5/60.0);
        lat += (grid.at(5).toLatin1() - 'A') * (2.5/60.0);

        // move to the center
        lon += 2.5/60;
        lat += 1.25/60;
    }
    else {
        // move to the center
        lon += 1;
        lat += 0.5;
    }

    return true;
}

double coordDistance(double latA, double lonA, double latB, double lonB) {
    double dLat = (latB-latA)*M_PI/180;
    double dLon = (lonB-lonA)*M_PI/180;
    latA = latA*M_PI/180;
    latB = latB*M_PI/180;

    double a = sin(dLat/2) * sin(dLat/2) +
               sin(dLon/2) * sin(dLon/2) * cos(latA) * cos(latB);

    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return EARTH_RADIUS * c;
}

int coordBearing(double latA, double lonA, double latB, double lonB) {
    double dLon = (lonB-lonA)*M_PI/180;
    latA = latA*M_PI/180;
    latB = latB*M_PI/180;

    double y = sin(dLon) * cos(latB);
    double x = cos(latA) * sin(latB) - sin(latA) * cos(latB) * cos(dLon);

    return (int)(180*atan2(y, x)/M_PI + 360) % 360;
}

QString freqToBand(double freq) {
    if (freq <= 2.0 && freq >= 1.8) return "160m";
    else if (freq <= 3.8 && freq >= 3.5) return "80m";
    else if (freq <= 7.5 && freq >= 7.0) return "40m";
    else if (freq <= 10.150 && freq >= 10.1) return"30m";
    else if (freq <= 14.350 && freq >= 14.0) return "20m";
    else if (freq <= 18.168 && freq >= 18.068) return "17m";
    else if (freq <= 21.450 && freq >= 21.000) return "15m";
    else if (freq <= 24.990 && freq >= 24.890) return "12m";
    else if (freq <= 29.700 && freq >= 28.000) return "10m";
    else if (freq <= 52 && freq >= 50) return "6m";
    else if (freq <= 148 && freq >= 144) return "2m";
    else if (freq <= 440 && freq >= 430) return "70cm";
    else return QString();
}
