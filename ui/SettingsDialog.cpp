#include <QSettings>
#include <QStringListModel>
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "models/RigTypeModel.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    RigTypeModel* rigTypeModel = new RigTypeModel(this);
    ui->hamlibModelSelect->setModel(rigTypeModel);

    QStringListModel* rigModel = new QStringListModel();
    ui->rigListView->setModel(rigModel);

    readSettings();
}

void SettingsDialog::save() {
    writeSettings();
    accept();
}

void SettingsDialog::addRig() {
    if (ui->rigNameEdit->text().isEmpty()) return;

    QStringListModel* model = (QStringListModel*)ui->rigListView->model();
    QStringList rigs = model->stringList();
    rigs << ui->rigNameEdit->text();
    model->setStringList(rigs);
    ui->rigNameEdit->clear();
}

void SettingsDialog::deleteRig() {
    foreach (QModelIndex index, ui->rigListView->selectionModel()->selectedRows()) {
        ui->rigListView->model()->removeRow(index.row());
    }
    ui->rigListView->clearSelection();
}

void SettingsDialog::readSettings() {
    QSettings settings;
    ui->callsignEdit->setText(settings.value("operator/callsign").toString());
    ui->locatorEdit->setText(settings.value("operator/grid").toString());
    QStringList rigs = settings.value("operator/rigs").toStringList();
    ((QStringListModel*)ui->rigListView->model())->setStringList(rigs);

    ui->hamlibModelSelect->setCurrentIndex(settings.value("hamlib/modelrow").toInt());
    ui->hamlibPortEdit->setText(settings.value("hamlib/port").toString());
    ui->hamlibBaudEdit->setValue(settings.value("hamlib/baudrate").toInt());

    ui->hamQthUsernameEdit->setText(settings.value("hamqth/username").toString());
    ui->hamQthPasswordEdit->setText(settings.value("hamqth/password").toString());
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("operator/callsign", ui->callsignEdit->text());
    settings.setValue("operator/grid", ui->locatorEdit->text());
    QStringList rigs = ((QStringListModel*)ui->rigListView->model())->stringList();
    settings.setValue("operator/rigs", rigs);

    int row = ui->hamlibModelSelect->currentIndex();
    QModelIndex index = ui->hamlibModelSelect->model()->index(row, 0);
    settings.setValue("hamlib/model", index.internalId());
    settings.setValue("hamlib/modelrow", row);
    settings.setValue("hamlib/port", ui->hamlibPortEdit->text());
    settings.setValue("hamlib/baudrate", ui->hamlibBaudEdit->value());

    settings.setValue("hamqth/username", ui->hamQthUsernameEdit->text());
    settings.setValue("hamqth/password", ui->hamQthPasswordEdit->text());
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}
