#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTime>

namespace Ui {
class ClockWidget;
}

class ClockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClockWidget(QWidget *parent = nullptr);
    ~ClockWidget();

public slots:
    void updateClock();
    void updateSun();

private:
    Ui::ClockWidget *ui;

    QTime sunrise;
    QTime sunset;
};

#endif // CLOCKWIDGET_H
