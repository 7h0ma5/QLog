#include "RigWidget.h"
#include "ui_RigWidget.h"
#include "core/Rig.h"

RigWidget::RigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RigWidget)
{
    ui->setupUi(this);

    Rig* rig = Rig::instance();

    connect(rig, &Rig::frequencyChanged, this, &RigWidget::updateFrequency);
    connect(rig, &Rig::modeChanged, this, &RigWidget::updateMode);
}

RigWidget::~RigWidget()
{
    delete ui;
}

void RigWidget::updateFrequency(double freq) {
    ui->freqLabel->setText(QString("%1 MHz").arg(QString::number(freq, 'f', 4)));
}

void RigWidget::updateMode(QString mode) {
    ui->modeLabel->setText(mode);
}
