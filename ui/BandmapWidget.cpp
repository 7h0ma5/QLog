#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "BandmapWidget.h"
#include "ui_BandmapWidget.h"
#include "core/Rig.h"
#include "data/Data.h"

BandmapWidget::BandmapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BandmapWidget)
{
    ui->setupUi(this);

    band = Data::band(14.100);

    bandmapScene = new QGraphicsScene(this);
    bandmapScene->setSceneRect(-50, 0, 300, 1000);
    ui->graphicsView->setScene(bandmapScene);
    ui->graphicsView->setStyleSheet("background-color: transparent;");

    Rig* rig = Rig::instance();
    connect(rig, &Rig::frequencyChanged, this, &BandmapWidget::updateRxFrequency);

    update();
}

void BandmapWidget::update() {
    bandmapScene->clear();

    // Draw Scale
    double step = 0.001;
    int steps = static_cast<int>(round((band.end - band.start) / step));
    bandmapScene->setSceneRect(-50, -10, 250, steps*10 + 20);
    ui->graphicsView->setFixedSize(300, steps*10 + 30);

    for (int i = 0; i <= steps; i++) {
        bandmapScene->addLine(0, i*10, (i % 5 == 0) ? 15 : 10, i*10);

        if (i % 5 == 0) {
            QGraphicsTextItem* text = bandmapScene->addText(QString::number(band.start + step*i, 'f', 3));
            text->setPos(- (text->boundingRect().width()) - 10, i*10 - (text->boundingRect().height() / 2));
        }
    }

    double y = ((rx_freq - band.start) / step) * 10;
    QPolygonF poly;
    poly << QPointF(-1, y) << QPointF(-7, y-5) << QPointF(-7, y+5);
    bandmapScene->addPolygon(poly, QPen(Qt::NoPen), QBrush(QColor(0, 255, 0), Qt::SolidPattern));

    QMap<double, DxSpot>::const_iterator lower = spots.lowerBound(band.start);
    QMap<double, DxSpot>::const_iterator upper = spots.upperBound(band.end);

    for (; lower != upper; lower++) {
        double y = ((lower.key() - band.start) / step) * 10;
        bandmapScene->addLine(17, y, 100, y);

        QGraphicsItem* text = bandmapScene->addText(lower.value().callsign);
        text->setPos(100, y - (text->boundingRect().height() / 2));
    }
}

void BandmapWidget::addSpot(DxSpot spot) {
    spots.insert(spot.freq, spot);
    update();
}

void BandmapWidget::updateRxFrequency(double freq) {
    rx_freq = freq;

    if (rx_freq < band.start || rx_freq > band.end) {
        Band newBand = Data::band(rx_freq);
        if (!newBand.name.isEmpty()) {
            band = newBand;
        }
    }

    update();
}

BandmapWidget::~BandmapWidget()
{
    delete ui;
}
