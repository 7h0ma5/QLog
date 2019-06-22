#ifndef WSJTXWIDGET_H
#define WSJTXWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>
#include "core/Wsjtx.h"

namespace Ui {
class WsjtxWidget;
}

class WsjtxTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    WsjtxTableModel(QObject* parent = 0) : QAbstractTableModel(parent) {}
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addEntry(QStringList entry);
    void clear();

private:
    QList<QStringList> wsjtxData;
};

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
    WsjtxTableModel* wsjtxTableModel;
    Ui::WsjtxWidget *ui;
};

#endif // WSJTXWIDGET_H
