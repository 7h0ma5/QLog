#ifndef DATA_H
#define DATA_H

#include <QtCore>
#include "Dxcc.h"
#include "Band.h"

class Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(QObject *parent = nullptr);
    static Data* instance();

    static DxccStatus dxccStatus(int dxcc, QString band, QString mode);
    static Band band(double freq);
    static QString freqToMode(double freq);
    static QColor statusToColor(DxccStatus status, QColor defaultColor);
    static QColor statusToInverseColor(DxccStatus status, QColor defaultColor);

    QStringList contestList() { return contests.values(); }
    QStringList propagationModesList() { return propagationModes.values(); }
    DxccEntity lookupDxcc(QString callsign);
    QString dxccFlag(int dxcc);
    QPair<QString, QString> legacyMode(QString mode);

signals:

public slots:

private:
    void loadContests();
    void loadPropagationModes();
    void loadLegacyModes();
    void loadDxccFlags();

    QMap<int, QString> flags;
    QMap<QString, QString> contests;
    QMap<QString, QString> propagationModes;
    QMap<QString, QPair<QString, QString>> legacyModes;
};

#endif // DATA_H
