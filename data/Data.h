#ifndef DATA_H
#define DATA_H

#include <QtCore>
#include "Dxcc.h"

class Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(QObject *parent = nullptr);
    static Data* instance();

    static DxccStatus dxccStatus(int dxcc, QString band, QString mode);
    static QString band(double freq);

    QStringList contestList() { return contests.values(); }
    QStringList propagationModesList() { return propagationModes.values(); }
    DxccEntity lookupDxcc(QString callsign);

signals:

public slots:

private:
    void loadContests();
    void loadPropagationModes();

    QMap<QString, QString> contests;
    QMap<QString, QString> propagationModes;
};

#endif // DATA_H
