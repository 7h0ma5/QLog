#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent* event);

signals:
    void settingsChanged();

private slots:
    void rigConnect();
    void showSettings();
    void importLog();
    void exportLog();
    void showAbout();

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
