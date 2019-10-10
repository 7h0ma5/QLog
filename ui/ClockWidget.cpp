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

    sunScene->setSceneRect(0, 0, 200, 10);
    ui->sunGraphicsView->setScene(sunScene);
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

    ui->sunRiseLabel->setText(sunrise.toString() + " UTC");
    ui->sunSetLabel->setText(sunset.toString() + " UTC");
}

void ClockWidget::updateSunGraph() {
    QColor dayColor(255, 253, 59);
    QColor nightColor(33, 150, 243);
    QColor currentColor(229, 57, 53);

    qreal width = sunScene->width();

    double rise = sunrise.msecsSinceStartOfDay() / MSECS_PER_DAY * width;
    double set = sunset.msecsSinceStartOfDay() / MSECS_PER_DAY * width;
    double cur = QDateTime::currentDateTimeUtc().time().msecsSinceStartOfDay() / MSECS_PER_DAY * width;

    sunScene->clear();

    if (set > rise) {
        sunScene->addRect(0, 0, rise, 10, QPen(Qt::NoPen), QBrush(nightColor, Qt::SolidPattern));
        sunScene->addRect(rise, 0, set-rise, 10, QPen(Qt::NoPen), QBrush(dayColor, Qt::SolidPattern));
        sunScene->addRect(set, 0, width-set, 10, QPen(Qt::NoPen), QBrush(nightColor, Qt::SolidPattern));
    }
    else {
        sunScene->addRect(0, 0, set, 10, QPen(Qt::NoPen), QBrush(dayColor, Qt::SolidPattern));
        sunScene->addRect(set, 0, rise-set, 10, QPen(Qt::NoPen), QBrush(nightColor, Qt::SolidPattern));
        sunScene->addRect(rise, 0, width-rise, 10, QPen(Qt::NoPen), QBrush(dayColor, Qt::SolidPattern));
    }

    QPen currentPen(currentColor);
    currentPen.setWidthF(2.0);
    sunScene->addLine(cur, 0, cur, 10, currentPen);
}

ClockWidget::~ClockWidget()
{
    delete ui;
}
