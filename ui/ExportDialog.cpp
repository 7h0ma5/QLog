#include <QFileDialog>
#include <QDebug>
#include "ui/ExportDialog.h"
#include "ui_ExportDialog.h"
#include "logformat/LogFormat.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
}

void ExportDialog::browse() {
    QString filename = QFileDialog::getSaveFileName(this);
    ui->fileEdit->setText(filename);
}

void ExportDialog::toggleAll() {
    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ExportDialog::runExport() {
    QFile file(ui->fileEdit->text());
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);

    LogFormat* format = LogFormat::open(ui->typeSelect->currentText(), out);

    if (!format) {
        qCritical() << "unknown log format";
        return;
    }

    if (!ui->allCheckBox->isChecked()) {
        format->setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    int count = format->runExport();

    delete format;

    ui->statusLabel->setText(tr("Exported %n contacts.", "", count));
}

ExportDialog::~ExportDialog() {
    delete ui;
}
