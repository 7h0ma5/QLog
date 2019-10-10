#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTemporaryFile>
#include <QSettings>
#include "LotwDialog.h"
#include "ui_LotwDialog.h"
#include "logformat/AdiFormat.h"
#include "core/Lotw.h"

LotwDialog::LotwDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LotwDialog)
{
    ui->setupUi(this);

    ui->qslRadioButton->setChecked(true);
    ui->qsoRadioButton->setChecked(false);

    QSettings settings;
    if (settings.value("lotw/last_update").isValid()) {
        QDate last_update = settings.value("lotw/last_update").toDate();
        ui->dateEdit->setDate(last_update);
    }
    else {
        ui->dateEdit->setDate(QDateTime::currentDateTimeUtc().date());
    }
}

void LotwDialog::download() {
    QProgressDialog* dialog = new QProgressDialog(tr("Updating from LotW"), tr("Cancel"), 0, 0, this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->setRange(0, 0);
    dialog->show();

    bool qsl = ui->qslRadioButton->isChecked();

    Lotw* lotw = new Lotw(dialog);
    connect(lotw, &Lotw::updateProgress, dialog, &QProgressDialog::setValue);
    connect(lotw, &Lotw::updateStarted, [dialog] {
        dialog->setRange(0, 100);
    });
    connect(lotw, &Lotw::updateComplete, [this, dialog, qsl](LotwUpdate update) {
        if (qsl) {
            QSettings settings;
            settings.setValue("lotw/last_update", QDateTime::currentDateTimeUtc().date());
        }
        dialog->close();
        QMessageBox::information(this, tr("QLog Error"),
                                 tr("LotW Update completed.") + "\n" +
                                 tr("QSOs checked: %n", "", update.qsos_checked) + "\n" +
                                 tr("New QSLs received: %n", "", update.qsls_updated) + "\n" +
                                 tr("QSOs updated: %n", "", update.qsos_updated) + "\n" +
                                 tr("Unmatched QSOs: %n", "", update.qsos_unmatched));
    });
    connect(lotw, &Lotw::updateFailed, [this, dialog]() {
        dialog->close();
        QMessageBox::critical(this, tr("QLog Error"), tr("LotW Update failed."));
    });

    lotw->update(ui->dateEdit->date(), ui->qsoRadioButton->isChecked());

    dialog->exec();
}

void LotwDialog::upload() {
    QTemporaryFile file;
    file.open();

    QSqlQuery query("SELECT * FROM contacts WHERE NOT lotw_qsl_sent = 'Y'");

    QTextStream stream(&file);
    AdiFormat adi(stream);

    int count = 0;

    while (query.next()) {
        QSqlRecord record = query.record();
        adi.exportContact(record);
        count++;
    }

    stream.flush();

    if (count > 0) {
        QProcess::execute("tqsl -d -q -u " + file.fileName());
        QMessageBox::information(this, tr("QLog Information"), tr("%n QSO(s) uploaded.", "", count));
    }
    else {
        QMessageBox::information(this, tr("QLog Information"), tr("No QSOs found to upload."));
    }
}

LotwDialog::~LotwDialog()
{
    delete ui;
}
