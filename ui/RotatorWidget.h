#ifndef ROTATORWIDGET_H
#define ROTATORWIDGET_H

#include <QWidget>

namespace Ui {
class RotatorWidget;
}

class QGraphicsScene;
class QGraphicsPathItem;

class RotatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RotatorWidget(QWidget *parent = nullptr);
    ~RotatorWidget();

public slots:
    void gotoPosition();
    void positionChanged(int azimuth, int elevation);

protected:
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    QGraphicsPathItem* compassNeedle;
    QGraphicsScene* compassScene;
    Ui::RotatorWidget *ui;
};

#endif // ROTATORWIDGET_H
