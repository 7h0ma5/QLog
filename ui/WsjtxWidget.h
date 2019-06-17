#ifndef WSJTXWIDGET_H
#define WSJTXWIDGET_H

#include <QWidget>
#include "core/Wsjtx.h"

namespace Ui {
class WsjtxWidget;
}

class WsjtxWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WsjtxWidget(QWidget *parent = nullptr);
    ~WsjtxWidget();

public slots:
    void decodeReceived(WsjtxDecode);
    void statusReceived(WsjtxStatus);

private:
    Ui::WsjtxWidget *ui;
};

#endif // WSJTXWIDGET_H
