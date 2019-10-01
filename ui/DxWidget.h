#ifndef DXWIDGET_H
#define DXWIDGET_H

#include <QWidget>
#include <QtNetwork>

#include "data/Data.h"
#include "data/DxSpot.h"

namespace Ui {
class DxWidget;
}

class DxTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    DxTableModel(QObject* parent = 0) : QAbstractTableModel(parent) {}
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addEntry(DxSpot entry);
    QString getCallsign(const QModelIndex& index);
    double getFrequency(const QModelIndex& index);
    void clear();

private:
    QList<DxSpot> dxData;
};

class DxWidget : public QWidget {
    Q_OBJECT

public:
    explicit DxWidget(QWidget *parent = 0);
    ~DxWidget();

public slots:
    void toggleConnect();
    void receive();
    void send();
    void connected();
    void socketError(QAbstractSocket::SocketError error);
    void rawModeChanged();
    void entryDoubleClicked(QModelIndex index);

signals:
    void tuneDx(QString callsign, double freq);
    void newSpot(DxSpot spot);

private:
    DxTableModel* dxTableModel;
    QTcpSocket* socket;
    Ui::DxWidget *ui;

    void connectCluster();
    void disconnectCluster();
};

#endif // DXWIDGET_H
