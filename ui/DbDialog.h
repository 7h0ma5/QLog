#ifndef DBDIALOG_H
#define DBDIALOG_H

#include <QDialog>

namespace Ui {
class DbDialog;
}

class DbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbDialog(QWidget *parent = 0);
    ~DbDialog();

public slots:
    void connect();
    void quit();

private:
    Ui::DbDialog *ui;
};

#endif // DBDIALOG_H
