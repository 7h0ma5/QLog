#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "BandmapWidget.h"
#include "ui_BandmapWidget.h"

BandmapWidget::BandmapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BandmapWidget)
{
    ui->setupUi(this);

    start = 14.000;
    end = 14.350;

    bandmapScene = new QGraphicsScene(this);
    bandmapScene->setSceneRect(-100, 0, 400, 1000);
    ui->graphicsView->setScene(bandmapScene);
    ui->graphicsView->setStyleSheet("background-color: transparent;");

    update();
}

void BandmapWidget::update() {
    bandmapScene->clear();

    // Draw Scale
    double step = (end - start) / 100;

    for (int i = 0; i < 100; i++) {
        bandmapScene->addLine(0, i*10, (i % 5 == 0) ? 15 : 10, i*10);

        if (i % 5 == 0) {
            QGraphicsTextItem* text = bandmapScene->addText(QString::number(start + step*i, 'f', 3));
            text->setPos(- (text->boundingRect().width()), i*10 - (text->boundingRect().height() / 2));
        }
    }

    QMap<double, DxSpot>::const_iterator lower = spots.lowerBound(start);
    QMap<double, DxSpot>::const_iterator upper = spots.upperBound(end);

    for (; lower != upper; lower++) {
        double y = ((lower.key() - start) / step) * 10;
        bandmapScene->addLine(17, y, 100, y);

        QGraphicsItem* text = bandmapScene->addText(lower.value().callsign);
        text->setPos(100, y - (text->boundingRect().height() / 2));
    }
}

void BandmapWidget::addSpot(DxSpot spot) {
    spots.insert(spot.freq, spot);
    update();
}

BandmapWidget::~BandmapWidget()
{
    delete ui;
}
