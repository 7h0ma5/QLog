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
        WsjtxEntry entry = wsjtxData.at(index.row());
        switch (index.column()) {
        case 0: return QString::number(entry.decode.snr);
        case 1: return entry.callsign;
        case 2: return entry.grid;
        case 3: return entry.dxcc.country;
        default: return QVariant();
        }
    }
    else if (index.column() == 1 && role == Qt::BackgroundRole) {
        WsjtxEntry entry = wsjtxData.at(index.row());
        switch (entry.status) {
        case DxccStatus::NewEntity:
            return QColor(Qt::red);
        case DxccStatus::NewBand:
        case DxccStatus::NewMode:
        case DxccStatus::NewBandMode:
            return QColor(Qt::blue);
        case DxccStatus::NewSlot:
            return QColor(Qt::green);

        default:
            return QVariant();
        }
    }
    else if (index.column() == 1 && role == Qt::TextColorRole) {
        WsjtxEntry entry = wsjtxData.at(index.row());
        switch (entry.status) {
        case DxccStatus::NewEntity:
            return QColor(Qt::white);
        case DxccStatus::NewBand:
        case DxccStatus::NewMode:
        case DxccStatus::NewBandMode:
            return QColor(Qt::white);
        case DxccStatus::NewSlot:
            return QColor(Qt::black);
        default:
            return QVariant();
        }
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

void WsjtxTableModel::addEntry(WsjtxEntry entry) {
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
    if (decode.message.startsWith("CQ")) {
        QRegExp cqRegExp("^CQ (DX )?([A-Z0-9\/]+) ?([A-Z]{2}[0-9]{2})?");
        if (cqRegExp.exactMatch(decode.message)) {
            WsjtxEntry entry;
            entry.decode = decode;
            entry.callsign = cqRegExp.cap(2);
            entry.grid = cqRegExp.cap(3);
            entry.dxcc = Data::instance()->lookupDxcc(entry.callsign);
            entry.status = Data::instance()->dxccStatus(entry.dxcc.dxcc, band, status.mode);
            wsjtxTableModel->addEntry(entry);
            ui->tableView->repaint();
        }
    }
}

void WsjtxWidget::statusReceived(WsjtxStatus newStatus) {
    if (this->status.dial_freq != newStatus.dial_freq) {
        band = Data::instance()->band(newStatus.dial_freq/1e6);
        ui->freqLabel->setText(QString("%1 MHz").arg(newStatus.dial_freq/1e6));
    }

    status = newStatus;

    ui->modeLabel->setText(status.mode);

    if (status.decoding) {
        wsjtxTableModel->clear();
    }
}

WsjtxWidget::~WsjtxWidget()
{
    delete ui;
}
