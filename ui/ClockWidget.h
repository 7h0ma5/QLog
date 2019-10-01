#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTime>

namespace Ui {
class ClockWidget;
}

class QGraphicsScene;

class ClockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClockWidget(QWidget *parent = nullptr);
    ~ClockWidget();

public slots:
    void updateClock();
    void updateSun();
    void updateSunGraph();

private:
    Ui::ClockWidget *ui;

    QGraphicsScene* sunScene;

    QTime sunrise;
    QTime sunset;
};

#endif // CLOCKWIDGET_H
