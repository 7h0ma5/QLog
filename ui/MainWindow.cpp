#include <QSettings>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QLabel>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/SettingsDialog.h"
#include "ui/ImportDialog.h"
#include "ui/ExportDialog.h"
#include "ui/StatisticsWidget.h"
#include "core/Fldigi.h"
#include "core/Lotw.h"
#include "core/Rig.h"
#include "core/Wsjtx.h"
#include "core/ClubLog.h"
#include "data/Data.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings;

    // restore the window geometry
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    QString op = settings.value("station/callsign", "NOCALL").toString();
    QString grid  = settings.value("station/grid", "NO GRID").toString();

    ui->statusBar->addWidget(new QLabel(op, ui->statusBar));
    ui->statusBar->addWidget(new QLabel(grid, ui->statusBar));

/*
    QMenu* trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->actionQuit);

    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
    trayIcon->showMessage("Hello", "This is a test", QIcon());
*/

    Fldigi* fldigi = new Fldigi(this);
    connect(fldigi, SIGNAL(contactAdded()), ui->logbookWidget, SLOT(updateTable()));

    Wsjtx* wsjtx = new Wsjtx(this);
    connect(wsjtx, &Wsjtx::statusReceived, ui->wsjtxWidget, &WsjtxWidget::statusReceived);
    connect(wsjtx, &Wsjtx::decodeReceived, ui->wsjtxWidget, &WsjtxWidget::decodeReceived);
    connect(wsjtx, &Wsjtx::contactAdded, ui->logbookWidget, &LogbookWidget::updateTable);
    connect(ui->wsjtxWidget, &WsjtxWidget::reply, wsjtx, &Wsjtx::startReply);

    ClubLog* clublog = new ClubLog(this);
    connect(wsjtx, &Wsjtx::contactAdded, clublog, &ClubLog::uploadContact);

    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->logbookWidget, &LogbookWidget::updateTable);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, clublog, &ClubLog::uploadContact);
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

void MainWindow::showStatistics() {
    qDebug() << "statistics";
    StatisticsWidget* stats = new StatisticsWidget();
    stats->show();
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

void MainWindow::updateLotw() {
    Lotw* lotw = new Lotw(this);
    lotw->update();
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
