#include <QSettings>
#include <QStringListModel>
#include <QSqlTableModel>
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

    modeTableModel = new QSqlTableModel(this);
    modeTableModel->setTable("modes");
    modeTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    modeTableModel->setSort(1, Qt::DescendingOrder);
    modeTableModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    modeTableModel->setHeaderData(3, Qt::Horizontal, tr("Report"));
    modeTableModel->setHeaderData(4, Qt::Horizontal, tr("DXCC"));
    modeTableModel->setHeaderData(5, Qt::Horizontal, tr("Enabled"));
    ui->modeTableView->setModel(modeTableModel);

    ui->modeTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->modeTableView->hideColumn(0);
    ui->modeTableView->hideColumn(2);

    modeTableModel->select();

    bandTableModel = new QSqlTableModel(this);
    bandTableModel->setTable("bands");
    bandTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    bandTableModel->setSort(2, Qt::AscendingOrder);
    bandTableModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    bandTableModel->setHeaderData(2, Qt::Horizontal, tr("Start"));
    bandTableModel->setHeaderData(3, Qt::Horizontal, tr("End"));
    bandTableModel->setHeaderData(4, Qt::Horizontal, tr("Enabled"));
    ui->bandTableView->setModel(bandTableModel);

    ui->bandTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->bandTableView->hideColumn(0);

    bandTableModel->select();

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
    ui->callsignEdit->setText(settings.value("station/callsign").toString());
    ui->locatorEdit->setText(settings.value("station/grid").toString());
    ui->operatorEdit->setText(settings.value("station/operator").toString());
    QStringList rigs = settings.value("station/rigs").toStringList();
    ((QStringListModel*)ui->rigListView->model())->setStringList(rigs);

    ui->hamlibModelSelect->setCurrentIndex(settings.value("hamlib/modelrow").toInt());
    ui->hamlibPortEdit->setText(settings.value("hamlib/port").toString());
    ui->hamlibBaudEdit->setValue(settings.value("hamlib/baudrate").toInt());

    ui->hamQthUsernameEdit->setText(settings.value("hamqth/username").toString());
    ui->hamQthPasswordEdit->setText(settings.value("hamqth/password").toString());

    ui->lotwUsernameEdit->setText(settings.value("lotw/username").toString());
    ui->lotwPasswordEdit->setText(settings.value("lotw/password").toString());
}

void SettingsDialog::writeSettings() {
    QSettings settings;

    settings.setValue("station/callsign", ui->callsignEdit->text());
    settings.setValue("station/grid", ui->locatorEdit->text());
    settings.setValue("station/operator", ui->operatorEdit->text());
    QStringList rigs = ((QStringListModel*)ui->rigListView->model())->stringList();
    settings.setValue("station/rigs", rigs);

    int row = ui->hamlibModelSelect->currentIndex();
    QModelIndex index = ui->hamlibModelSelect->model()->index(row, 0);
    settings.setValue("hamlib/model", index.internalId());
    settings.setValue("hamlib/modelrow", row);
    settings.setValue("hamlib/port", ui->hamlibPortEdit->text());
    settings.setValue("hamlib/baudrate", ui->hamlibBaudEdit->value());

    settings.setValue("hamqth/username", ui->hamQthUsernameEdit->text());
    settings.setValue("hamqth/password", ui->hamQthPasswordEdit->text());

    settings.setValue("lotw/username", ui->lotwUsernameEdit->text());
    settings.setValue("lotw/password", ui->lotwPasswordEdit->text());
}

SettingsDialog::~SettingsDialog() {
    delete modeTableModel;
    delete bandTableModel;
    delete ui;
}
