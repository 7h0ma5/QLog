#ifndef LOOKBOOKWIDGET_H
#define LOOKBOOKWIDGET_H

#include <QWidget>

namespace Ui {
class LogbookWidget;
}

class QSqlTableModel;

class LogbookWidget : public QWidget {
    Q_OBJECT

public:
    explicit LogbookWidget(QWidget *parent = 0);
    ~LogbookWidget();

public slots:
    void callsignFilterChanged();
    void bandFilterChanged();
    void updateTable();
    void deleteContact();

private:
    QSqlTableModel* model;
    Ui::LogbookWidget *ui;
};

#endif // LOGBOOKWIDGET_H
