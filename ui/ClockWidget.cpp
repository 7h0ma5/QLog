#include <QTimer>
#include <QDateTime>
#include "ClockWidget.h"
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
}

void ClockWidget::updateClock() {
    QDateTime now = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    ui->clockLabel->setText(now.toString("HH:mm:ss") + " UTC");
}

ClockWidget::~ClockWidget()
{
    delete ui;
}
