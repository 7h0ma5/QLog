#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class QSqlTableModel;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void save();
    void addRig();
    void deleteRig();

private:
    void readSettings();
    void writeSettings();

    QSqlTableModel* modeTableModel;
    QSqlTableModel* bandTableModel;
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
