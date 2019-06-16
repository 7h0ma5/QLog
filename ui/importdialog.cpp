#include <QFileDialog>
#include "importdialog.h"
#include "ui_importdialog.h"
#include "logformat/logformat.h"

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    QSettings settings;

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->gridEdit->setText(settings.value("operator/grid").toString());
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    //ui->progressBar->setDisabled(true);

    QStringList rigs = settings.value("operator/rigs").toStringList();
    QStringListModel* rigModel = new QStringListModel(rigs, this);
    ui->rigSelect->setModel(rigModel);
    if (!ui->rigSelect->currentText().isEmpty()) {
        ui->rigCheckBox->setChecked(true);
    }
}

void ImportDialog::browse() {
    QString filename = QFileDialog::getOpenFileName(this, "ADIF File", "", "*.adi");
    ui->fileEdit->setText(filename);
}

void ImportDialog::toggleAll() {
    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ImportDialog::progress(qint64 value) {
    int progress = (int)(value * 100 / size);
    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}

void ImportDialog::runImport() {
    QFile file(ui->fileEdit->text());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);

    size = file.size();

    QMap<QString, QString> defaults;

    if (ui->gridCheckBox->isChecked()) {
        defaults["my_gridsquare"] = ui->gridEdit->text();
    }

    if (ui->rigCheckBox->isChecked()) {
        defaults["my_rig"] = ui->rigSelect->currentText();
    }

    if (ui->commentCheckBox->isChecked()) {
        defaults["comment"] = ui->commentEdit->text();
    }

    LogFormat* format = LogFormat::open(ui->typeSelect->currentText(), in);
    format->setDefaults(defaults);

    if (!format) {
        qCritical() << "unknown log format";
        return;
    }

    if (!ui->allCheckBox->isChecked()) {
        format->setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    /*
    QThread* thread = new QThread;
    format->moveToThread(thread);
    */

    connect(format, &LogFormat::progress, this, &ImportDialog::progress);
    /*
    connect(thread, &QThread::started, format, &LogFormat::runImport);
    connect(format, &LogFormat::finished, thread, &QThread::quit);
    connect(format, &LogFormat::finished, format, &LogFormat::deleteLater);
    connect(format, &LogFormat::finished, thread, &QThread::deleteLater);

    thread->start();
    */

    ui->buttonBox->setEnabled(false);
    format->runImport();
    ui->buttonBox->setEnabled(true);

    this->close();
    //ui->statusLabel->setText(tr("Imported %n contacts.", "", count));
}

ImportDialog::~ImportDialog()
{
    delete ui;
}
