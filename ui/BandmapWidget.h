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

    enum BandmapZoom {
        ZOOM_100HZ,
        ZOOM_250HZ,
        ZOOM_500HZ,
        ZOOM_1KHZ,
        ZOOM_2K5HZ,
        ZOOM_5KHZ,
        ZOOM_10KHZ
    };

public slots:
    void update();
    void updateRxFrequency(double freq);
    void addSpot(DxSpot spot);
    void clearSpots();
    void zoomIn();
    void zoomOut();

private:
    void removeDuplicates(DxSpot &spot);

private:
    Ui::BandmapWidget *ui;

    double rx_freq, tx_freq;
    Band band;
    BandmapZoom zoom;
    QGraphicsScene* bandmapScene;
    QMap<double, DxSpot> spots;
};

#endif // BANDMAPWIDGET_H
