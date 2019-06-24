#ifndef DATA_H
#define DATA_H

#include <QtCore>
#include "Band.h"
#include "Mode.h"
#include "Dxcc.h"

class Cty;

class Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(QObject *parent = nullptr);
    static Data* instance();

    Band band(float freq);
    Mode mode(QString mode) { return modes.value(mode); }
    QStringList bandList() { return bands.keys(); }
    QStringList modeList() { return modes.keys(); }
    QStringList contestList() { return contests.values(); }
    QStringList propagationModesList() { return propagationModes.values(); }
    DxccEntity lookupDxcc(QString callsign);

signals:

public slots:

private:
    void loadBands();
    void loadModes();
    void loadContests();
    void loadPropagationModes();

    QMap<QString, Band> bands;
    QMap<QString, Mode> modes;
    QMap<QString, QString> contests;
    QMap<QString, QString> propagationModes;
    Cty* cty;
};

#endif // DATA_H
