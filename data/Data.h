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

    QStringList contestList() { return contests.values(); }
    QStringList propagationModesList() { return propagationModes.values(); }
    DxccEntity lookupDxcc(QString callsign);
    QString dxccFlag(int dxcc);

signals:

public slots:

private:
    void loadContests();
    void loadPropagationModes();
    void loadDxccFlags();

    QMap<int, QString> flags;
    QMap<QString, QString> contests;
    QMap<QString, QString> propagationModes;
};

#endif // DATA_H
