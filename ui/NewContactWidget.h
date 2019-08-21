#ifndef NEWCONTACTWIDGET_H
#define NEWCONTACTWIDGET_H

#include <QWidget>
#include <QSqlRecord>
#include "core/HamQTH.h"
#include "core/Cty.h"
#include "data/Data.h"

namespace Ui {
class NewContactWidget;
}

class HamQTH;
class Rig;

enum CoordPrecision {
    COORD_NONE = 0,
    COORD_DXCC = 1,
    COORD_GRID = 2,
    COORD_FULL = 3
};

class NewContactWidget : public QWidget {
    Q_OBJECT

public:
    explicit NewContactWidget(QWidget *parent = 0);
    ~NewContactWidget();

signals:
    void contactAdded(QSqlRecord record);
    void newTarget(double lat, double lon);

public slots:
    void reloadSettings();
    void callsignChanged();
    void frequencyChanged();
    void bandChanged();
    void modeChanged();
    void updateBand(double freq);
    void resetContact();
    void saveContact();
    void gridChanged();
    void updateTime();
    void updateTimeOff();
    void startContactTimer();
    void stopContactTimer();
    void callsignResult(const QMap<QString, QString>& data);
    void updateCoordinates(double lat, double lon, CoordPrecision prec);
    void updateDxccStatus();
    void changeFrequency(double freq);
    void changeMode(QString mode);
    void changePower(double power);
    void tuneDx(QString callsign, double frequency);
    void setDefaultReport();
    void qrz();

private:
    void queryDatabase(QString callsign);
    void queryDxcc(QString callsign);
    void readSettings();
    void writeSettings();

private:
    Rig* rig;
    QString callsign;
    DxccEntity dxccEntity;
    QString defaultReport;
    HamQTH callbook;
    QTimer* contactTimer;
    Ui::NewContactWidget *ui;
    CoordPrecision coordPrec;
};

#endif // NEWCONTACTWIDGET_H
