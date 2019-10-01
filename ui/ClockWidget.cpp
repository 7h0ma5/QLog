#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include "ClockWidget.h"
#include "core/utils.h"
#include "ui_ClockWidget.h"

#define MSECS_PER_DAY (24.0 * 60.0 * 60.0 * 1000.0)

ClockWidget::ClockWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClockWidget)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ClockWidget::updateClock);
    timer->start(1000);

    sunScene = new QGraphicsScene(this);

    sunScene->setSceneRect(0, 0, 100, 10);
    ui->sunGraphicsView->setScene(sunScene);
//    ui->sunGraphicsView->setSceneRect(0, 0, 100, 10);
    ui->sunGraphicsView->fitInView(sunScene->sceneRect(), Qt::IgnoreAspectRatio);
    ui->sunGraphicsView->setStyleSheet("background-color: transparent;");

    updateClock();
    updateSun();
    updateSunGraph();
}

void ClockWidget::updateClock() {
    QDateTime now = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    ui->clockLabel->setText(now.toString("HH:mm:ss") + " UTC");

    if (now.time().second() == 0) {
        updateSunGraph();
    }
}

void ClockWidget::updateSun() {
    QSettings settings;
    QString grid = settings.value("station/grid").toString();

    double lat, lon;
    gridToCoord(grid, lat, lon);

    qint64 julianDay = QDate::currentDate().toJulianDay();
    double n = static_cast<double>(julianDay) - 2451545.0 + 0.0008;

    double Js = n - lon / 360.0;
    double M = fmod(357.5291 + 0.98560028 * Js, 360.0);
    double C = 1.9148 * sin(M / 180.0 * M_PI) + 0.0200 * sin(2 * M / 180.0 * M_PI) + 0.0003 * sin(3 * M / 180.0 * M_PI);
    double L = fmod(M + C + 180 + 102.9372, 360.0);
    double Jt = 2451545.0 + Js + 0.0053 * sin(M / 180.0 * M_PI) - 0.0069 * sin(2 * L / 180.0 * M_PI);
    double sind = sin(L / 180.0 * M_PI) * sin(23.44 / 180.0 * M_PI);
    double cosd = cos(asin(sind));
    double w = acos((sin(-0.83 / 180.0 * M_PI) - sin(lat / 180.0 * M_PI) * sind) / (cos(lat / 180.0 * M_PI) * cosd));

    double Jrise = Jt - w / (2*M_PI) + 0.5;
    double Jset = Jt + w / (2*M_PI) + 0.5;

    sunrise = QTime::fromMSecsSinceStartOfDay(static_cast<int>(fmod(Jrise, 1.0) * MSECS_PER_DAY));
    sunset = QTime::fromMSecsSinceStartOfDay(static_cast<int>(fmod(Jset, 1.0) * MSECS_PER_DAY));

    ui->sunRiseLabel->setText(sunrise.toString() + " Z");
    ui->sunSetLabel->setText(sunset.toString() + " Z");
}

void ClockWidget::updateSunGraph() {
    double rise_percent = sunrise.msecsSinceStartOfDay() / MSECS_PER_DAY * 100.0;
    double set_percent = sunset.msecsSinceStartOfDay() / MSECS_PER_DAY * 100.0;
    double cur_percent = QDateTime::currentDateTimeUtc().time().msecsSinceStartOfDay() / MSECS_PER_DAY * 100.0;

    sunScene->clear();
    sunScene->addRect(0, 0, rise_percent, 10, QPen(Qt::NoPen), QBrush(QColor(0, 0, 255), Qt::SolidPattern));
    sunScene->addRect(rise_percent, 0, set_percent-rise_percent, 10, QPen(Qt::NoPen), QBrush(QColor(255, 255, 0), Qt::SolidPattern));
    sunScene->addRect(set_percent, 0, 100-set_percent, 10, QPen(Qt::NoPen), QBrush(QColor(0, 0, 255), Qt::SolidPattern));
    sunScene->addLine(cur_percent, 0, cur_percent, 10, QPen(QColor(255, 0, 0)));
}

ClockWidget::~ClockWidget()
{
    delete ui;
}
