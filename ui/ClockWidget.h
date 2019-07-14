#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>

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

private:
    Ui::ClockWidget *ui;
};

#endif // CLOCKWIDGET_H
