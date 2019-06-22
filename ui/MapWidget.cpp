#include <QGraphicsTextItem>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QVector3D>
#include <cmath>
#include "core/utils.h"
#include "MapWidget.h"
#include "ui_MapWidget.h"

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->mapView->setScene(scene);
    ui->mapView->setStyleSheet("background-color: transparent;");

    QPixmap pix(":/res/map/nasabluemarble.jpg");
    scene->addPixmap(pix);
    scene->setSceneRect(pix.rect());

    nightOverlay = new QGraphicsPixmapItem();
    scene->addItem(nightOverlay);
    redrawNightOverlay();

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(redraw()));
    timer->start(60000);
}

void MapWidget::clear() {
    QMutableListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
        QGraphicsItem* item = i.next();
        scene->removeItem(item);
        i.remove();
    }
}

void MapWidget::redraw() {
    redrawNightOverlay();
}

void MapWidget::drawPoint(QPoint point) {
    items << scene->addEllipse(point.x()-2, point.y()-2, 4, 4,
                               QPen(QColor(255, 0, 0)),
                               QBrush(QColor(255, 0, 0),
                                      Qt::SolidPattern));
}

void MapWidget::drawLine(QPoint pointA, QPoint pointB) {
    QPainterPath path;
    path.moveTo(pointA);

    double latA, lonA, latB, lonB;
    pointToRad(pointA, latA, lonA);
    pointToRad(pointB, latB, lonB);

    double d = 2*asin(sqrt(pow(sin(latA-latB)/2, 2) + cos(latA)* cos(latB) * pow(sin((lonA-lonB)/2), 2)));

    for (double f = 0; f < 1; f += 0.0001) {
        double A = sin((1-f)*d)/sin(d);
        double B = sin(f*d)/sin(d);
        double x = A*cos(latA)*cos(lonA) + B*cos(latB)*cos(lonB);
        double y = A*cos(latA)*sin(lonA) + B*cos(latB)*sin(lonB);
        double z = A*sin(latA)           + B*sin(latB);
        double lat = atan2(z, sqrt(x*x + y*y));
        double lon = atan2(y, x);

        QPoint p = radToPoint(lat, lon);
        path.lineTo(p);
        path.moveTo(p);
    }

    path.lineTo(pointB);
    path.closeSubpath();

    items << scene->addPath(path, QPen(QColor(255, 0, 0)),
                            QBrush(QColor(255, 0, 0), Qt::SolidPattern));
}

void MapWidget::redrawNightOverlay() {
    QDateTime current = QDateTime::currentDateTimeUtc();
    int secondOfDay = (QTime(0, 0, 0).secsTo(current.time()) + 43200) % 86400;
    int dayOfYear = current.date().dayOfYear();
    int daysInYear = QDate::isLeapYear(current.date().year()) ? 366 : 365;
    int longestDay = QDate(current.date().year(), 6, 21).dayOfYear();

    double tilt = 23.5 * cos(2.0*M_PI*((double)(dayOfYear-longestDay)/daysInYear));

    double sunX = cos(2*M_PI*(secondOfDay/86400.0));
    double sunY = -sin(2*M_PI*(secondOfDay/86400.0));
    double sunZ = tan(2*M_PI*(tilt/360.0));

    QVector3D sun(sunX, sunY, sunZ);
    sun.normalize();

    // <plot sun position>
    double theta = acos(sunZ);
    double phi = atan(sunY/sunX);
    int lon = phi/M_PI * 180 - 180;
    int lat = 90 - theta/M_PI * 180;
    drawPoint(coordToPoint(lat, lon));
    // </plot sun position>

    int maxX = scene->width();
    int maxY = scene->height();

    QImage overlay(maxX, maxY, QImage::Format_ARGB32);
    uchar* buffer = overlay.bits();

    for (int y = 0; y < maxY; y++) {
        double theta = M_PI*((double)y/(maxY-1));
        double posZ = cos(theta);
        double sinTheta = sin(theta);

        for (int x = 0; x < maxX; x++) {
            double phi = 2*M_PI*((double)x/(maxX-1)) - M_PI;

            double posX = sinTheta*cos(phi);
            double posY = sinTheta*sin(phi);

            QVector3D pos(posX, posY, posZ);
            pos.normalize();

            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;

            double ill = QVector3D::dotProduct(sun, pos);
            if (ill <= -0.1) {
                buffer[3] = 255;
            }
            else if (ill < 0.1) {
                buffer[3] = 255-(ill+0.1)*5*255;
            }
            else {
                buffer[3] = 0;
            }
            buffer += 4;
        }
    }

    QImage night(":/res/map/nasaearthlights.jpg");
    night.convertToFormat(QImage::Format_ARGB32);

    QPainter painter;
    painter.begin(&overlay);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.drawImage(0, 0, night);
    painter.end();

    nightOverlay->setPixmap(QPixmap::fromImage(overlay));
}

void MapWidget::pointToRad(QPoint point, double& lat, double& lon) {
    lat = M_PI/2 - (double)point.y()/(scene->height()-1)*M_PI;
    lon = 2*M_PI*((double)point.x()/(scene->width()-1)) - M_PI;
}

void MapWidget::pointToCoord(QPoint point, double& lat, double& lon) {
    lat = 90 - (point.y()/scene->height())*180;
    lon = 360*(point.x()/scene->width()) - 180;
}

QPoint MapWidget::radToPoint(double lat, double lon) {
    int x = (lon+M_PI)/(2*M_PI)*scene->width();
    int y = scene->height()/2 - (2*lat/M_PI)*(scene->height()/2);
    return QPoint(x, y);
}

QPoint MapWidget::coordToPoint(double lat, double lon) {
    int x = (lon+180)/360*scene->width();
    int y = scene->height()/2 - (lat/90)*(scene->height()/2);
    return QPoint(x, y);
}

void MapWidget::setTarget(double lat, double lon) {
    clear();

    if (lat == 0.0 && lon == 0.0) return;

    QSettings settings;
    QString grid = settings.value("station/grid").toString();

    QPoint point = coordToPoint(lat, lon);
    drawPoint(point);

    double qthLat = 0, qthLon = 0;
    bool res = gridToCoord(grid, qthLat, qthLon);

    if (res) {
        QPoint qth = coordToPoint(qthLat, qthLon);
        drawPoint(qth);
        drawLine(qth, point);
    }
}

void MapWidget::showEvent(QShowEvent* event) {
    ui->mapView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    QWidget::showEvent(event);
}

void MapWidget::resizeEvent(QResizeEvent* event) {
    ui->mapView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    QWidget::resizeEvent(event);
}

MapWidget::~MapWidget()
{
    delete ui;
}
