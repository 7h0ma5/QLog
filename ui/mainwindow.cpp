#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/settingsdialog.h"
#include "ui/importdialog.h"
#include "ui/exportdialog.h"
#include "core/fldigi.h"
#include "core/rig.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings;

    // restore the window geometry
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    QString op = settings.value("operator/callsign", "NOCALL").toString();
    QString grid  = settings.value("operator/grid", "NO GRID").toString();

    ui->statusBar->addWidget(new QLabel(op, ui->statusBar));
    ui->statusBar->addWidget(new QLabel(grid, ui->statusBar));

    Fldigi* fldigi = new Fldigi(this);
    connect(fldigi, SIGNAL(contactAdded()), ui->logbookWidget, SLOT(updateTable()));
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;

    // save the window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::rigConnect() {
    Rig::instance()->open();
}

void MainWindow::showSettings() {
    SettingsDialog sw;
    if (sw.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

void MainWindow::importLog() {
    ImportDialog dialog;
    dialog.exec();
    ui->logbookWidget->updateTable();
}

void MainWindow::exportLog() {
    ExportDialog dialog;
    dialog.exec();
}

void MainWindow::showAbout() {
    QString aboutText = "<h1>QLog %1</h1>"
                        "<p>&copy; 2019 Thomas Gatzweiler DL2IC</p>"
                        "<p><a href='http://dl2ic.de'>www.dl2ic.de</a></p>"
                        "<p>Icon by <a href='http://www.iconshock.com'>Icon Shock</a><br />"
                        "Satellite images by <a href='http://www.nasa.gov'>NASA</p>";


    QString version = QCoreApplication::applicationVersion();
    aboutText = aboutText.arg(version);

    QMessageBox::about(this, "About", aboutText);
}

MainWindow::~MainWindow() {
    delete ui;
}
