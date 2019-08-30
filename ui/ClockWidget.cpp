#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include "ClockWidget.h"
#include "core/utils.h"
#include "ui_ClockWidget.h"

ClockWidget::ClockWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClockWidget)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ClockWidget::updateClock);
    timer->start(1000);

    updateClock();
    updateSun();
}

void ClockWidget::updateClock() {
    QDateTime now = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    ui->clockLabel->setText(now.toString("HH:mm:ss") + " UTC");
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

    QTime rise = QTime::fromMSecsSinceStartOfDay(static_cast<int>(fmod(Jrise, 1.0) * (24.0 * 60.0 * 60.0 * 1000.0)));
    QTime set = QTime::fromMSecsSinceStartOfDay(static_cast<int>(fmod(Jset, 1.0) * (24.0 * 60.0 * 60.0 * 1000.0)));

    ui->sunRiseLabel->setText(rise.toString());
    ui->sunSetLabel->setText(set.toString());
}

ClockWidget::~ClockWidget()
{
    delete ui;
}
