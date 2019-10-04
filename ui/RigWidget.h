#ifndef RIGWIDGET_H
#define RIGWIDGET_H

#include <QWidget>

namespace Ui {
class RigWidget;
}

class RigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RigWidget(QWidget *parent = nullptr);
    ~RigWidget();

public slots:
    void updateFrequency(double freq);
    void updateMode(QString mode);

private:
    Ui::RigWidget *ui;
};

#endif // RIGWIDGET_H
