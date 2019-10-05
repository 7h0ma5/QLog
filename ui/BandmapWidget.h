#ifndef BANDMAPWIDGET_H
#define BANDMAPWIDGET_H

#include <QWidget>
#include <QMap>
#include "data/DxSpot.h"
#include "data/Band.h"

namespace Ui {
class BandmapWidget;
}

class QGraphicsScene;

class BandmapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BandmapWidget(QWidget *parent = nullptr);
    ~BandmapWidget();

public slots:
    void update();
    void updateRxFrequency(double freq);
    void addSpot(DxSpot spot);

private:
    Ui::BandmapWidget *ui;

    double rx_freq, tx_freq;
    Band band;
    QGraphicsScene* bandmapScene;
    QMap<double, DxSpot> spots;
};

#endif // BANDMAPWIDGET_H
