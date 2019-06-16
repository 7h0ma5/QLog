#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

public slots:
    void browse();
    void toggleAll();
    void runImport();
    void progress(qint64 value);

private:
    Ui::ImportDialog *ui;
    qint64 size;
};

#endif // IMPORTDIALOG_H
