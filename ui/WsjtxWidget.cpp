#include <QDebug>
#include "WsjtxWidget.h"
#include "ui_WsjtxWidget.h"
#include "data/Data.h"

int WsjtxTableModel::rowCount(const QModelIndex&) const {
    return wsjtxData.count();
}

int WsjtxTableModel::columnCount(const QModelIndex&) const {
    return 4;
}

QVariant WsjtxTableModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        QStringList row = wsjtxData.at(index.row());
        return row.at(index.column());
    }
    return QVariant();
}

QVariant WsjtxTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section) {
    case 0: return tr("SNR");
    case 1: return tr("Callsign");
    case 2: return tr("Grid");
    case 3: return tr("Country");
    default: return QVariant();
    }
}

void WsjtxTableModel::addEntry(QStringList entry) {
    beginInsertRows(QModelIndex(), wsjtxData.count(), wsjtxData.count());
    wsjtxData.append(entry);
    endInsertRows();
}

void WsjtxTableModel::clear() {
    beginResetModel();
    wsjtxData.clear();
    endResetModel();
}

WsjtxWidget::WsjtxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WsjtxWidget)
{
    ui->setupUi(this);

    wsjtxTableModel = new WsjtxTableModel(this);
    ui->tableView->setModel(wsjtxTableModel);
}

void WsjtxWidget::decodeReceived(WsjtxDecode decode) {
    /*
    QTableWidgetItem *newItem = new QTableWidgetItem(decode.message);
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, newItem);
    qDebug() << "Decode received";
    */

    QStringList entry;

    if (decode.message.startsWith("CQ")) {
        QRegExp cqRegExp("^CQ (DX )?([A-Z0-9\/]+) ?([A-Z]{2}[0-9]{2})?");
        if (cqRegExp.exactMatch(decode.message)) {
            QString callsign = cqRegExp.cap(2);
            QString grid = cqRegExp.cap(3);
            DxccEntity dxcc = Data::instance()->lookupDxcc(callsign);

            entry << QString::number(decode.snr) << callsign << grid << dxcc.country;
            wsjtxTableModel->addEntry(entry);
            ui->tableView->repaint();
        }
    }
}

void WsjtxWidget::statusReceived(WsjtxStatus status) {
    ui->freqLabel->setText(QString("%1").arg(status.dial_freq));
    ui->modeLabel->setText(status.mode);

    if (status.decoding) {
        wsjtxTableModel->clear();
    }
}

WsjtxWidget::~WsjtxWidget()
{
    delete ui;
}
