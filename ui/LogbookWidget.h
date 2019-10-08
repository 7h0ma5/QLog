#ifndef LOOKBOOKWIDGET_H
#define LOOKBOOKWIDGET_H

#include <QWidget>

namespace Ui {
class LogbookWidget;
}

class ClubLog;
class LogbookModel;

class LogbookWidget : public QWidget {
    Q_OBJECT

public:
    explicit LogbookWidget(QWidget *parent = nullptr);
    ~LogbookWidget();

public slots:
    void filterCallsign(QString call);
    void filterSelectedCallsign();
    void lookupSelectedCallsign();
    void callsignFilterChanged();
    void bandFilterChanged();
    void updateTable();
    void uploadClublog();
    void deleteContact();

private:
    ClubLog* clublog;
    LogbookModel* model;
    Ui::LogbookWidget *ui;
};

#endif // LOGBOOKWIDGET_H
