#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "core/Rotator.h"
#include "core/utils.h"
#include "RotatorWidget.h"
#include "ui_RotatorWidget.h"

#define MAP_RESOLUTION 1000

RotatorWidget::RotatorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RotatorWidget)
{
    ui->setupUi(this);

    compassScene = new QGraphicsScene(this);
    ui->compassView->setScene(compassScene);
    ui->compassView->setStyleSheet("background-color: transparent;");

    compassScene->setSceneRect(-100, -100, 200, 200);

    QImage source(":/res/map/nasabluemarble.jpg");

    QImage map(MAP_RESOLUTION, MAP_RESOLUTION, QImage::Format_ARGB32);

    QSettings settings;
    QString grid = settings.value("station/grid").toString();

    double lat, lon;
    gridToCoord(grid, lat, lon);

    double lambda0 = (lon / 180.0) * (2.0 * M_PI);
    double phi1 = - (lat / 90.0) * (0.5 * M_PI);
    qDebug() << lambda0 << phi1;
    //double lambda0 = 0.2;
    //double phi1 = -0.8;

    for (int x = 0; x < map.width(); x++) {
        double x2 = 2.0 * M_PI * (static_cast<double>(x) / static_cast<double>(map.width()) - 0.5);
        for (int y = 0; y < map.height(); y++) {
            double y2 = 2.0 * M_PI * (static_cast<double>(y) / static_cast<double>(map.height()) - 0.5);
            double c = sqrt(x2*x2 + y2*y2);
            double phi = asin(cos(c) * sin(phi1) + y2 * sin(c) * cos(phi1) / c);

            if (c < M_PI) {
                double lambda = lambda0 + atan2(x2*sin(c), c*cos(phi1)*cos(c) - y2*sin(phi1)*sin(c));

                double s = (lambda/(2*M_PI)) + 0.5;
                double t = (phi/M_PI) + 0.5;

                int x3 = static_cast<int>(s * static_cast<double>(source.width())) % source.width();
                x3 = x3 < 0 ? x3 + source.width() : x3;

                int y3 = static_cast<int>(t * static_cast<double>(source.height())) % source.height();
                y3 = y3 < 0 ? y3 + source.width() : y3;

                map.setPixelColor(x, y, source.pixelColor(x3, y3));
            }
            else {
                map.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
    }

    QGraphicsPixmapItem* pixmapItem = compassScene->addPixmap(QPixmap::fromImage(map));
    pixmapItem->moveBy(-MAP_RESOLUTION/2, -MAP_RESOLUTION/2);
    pixmapItem->setTransformOriginPoint(MAP_RESOLUTION/2, MAP_RESOLUTION/2);
    pixmapItem->setScale(200.0/MAP_RESOLUTION);

    compassScene->addEllipse(-100, -100, 200, 200, QPen(QColor(100, 100, 100), 2),
                                             QBrush(QColor(0, 0, 0), Qt::NoBrush));

    compassScene->addEllipse(-1, -1, 2, 2, QPen(Qt::NoPen),
                                             QBrush(QColor(0, 0, 0), Qt::SolidPattern));

    QPainterPath path;
    path.lineTo(-1, 0);
    path.lineTo(0, -70);
    path.lineTo(1, 0);
    path.closeSubpath();
    compassNeedle = compassScene->addPath(path, QPen(Qt::NoPen),
                    QBrush(QColor(255, 255, 255), Qt::SolidPattern));

    //compassScene->addLine(0, 100, 0, 90);
    //compassScene->addLine(0, -100, 0, -90);
    //compassScene->addLine(100, 0, 90, 0);
    //compassScene->addLine(-100, 0, -90, 0);

    /*
    QGraphicsTextItem* north = compassScene->addText("N");
    north->adjustSize();
    north->setPos(-north->boundingRect().width()/2, -90);

    QGraphicsTextItem* south = compassScene->addText("S");
    south->adjustSize();
    south->setPos(-south->boundingRect().width()/2, 90 - south->boundingRect().height());

    QGraphicsTextItem* east = compassScene->addText("E");
    east->adjustSize();
    east->setPos(90 - east->boundingRect().width(), -east->boundingRect().height()/2);

    QGraphicsTextItem* west = compassScene->addText("W");
    west->adjustSize();
    west->setPos(-90, -west->boundingRect().height()/2);
    */

    connect(Rotator::instance(), &Rotator::positionChanged, this, &RotatorWidget::positionChanged);
}

void RotatorWidget::gotoPosition() {
    int azimuth = ui->gotoSpinBox->value();
    int elevation = 0;
    Rotator::instance()->setPosition(azimuth, elevation);
}


void RotatorWidget::positionChanged(int azimuth, int elevation) {
    compassNeedle->setRotation(azimuth);
}

void RotatorWidget::showEvent(QShowEvent* event) {
    ui->compassView->fitInView(compassScene->sceneRect(), Qt::KeepAspectRatio);
    QWidget::showEvent(event);
}

void RotatorWidget::resizeEvent(QResizeEvent* event) {
    ui->compassView->fitInView(compassScene->sceneRect(), Qt::KeepAspectRatio);
    QWidget::resizeEvent(event);
}

RotatorWidget::~RotatorWidget()
{
    delete ui;
}
