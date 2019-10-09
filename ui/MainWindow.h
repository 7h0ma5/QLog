#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Conditions;
class QLabel;

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
    void rotConnect();
    void showSettings();
    void showStatistics();
    void importLog();
    void exportLog();
    void showLotw();
    void showAbout();
    void conditionsUpdated();

private:
    Ui::MainWindow* ui;
    Conditions* conditions;
    QLabel* conditionsLabel;
};

#endif // MAINWINDOW_H
