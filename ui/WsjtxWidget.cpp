#include <QDebug>
#include "WsjtxWidget.h"
#include "ui_WsjtxWidget.h"
#include "data/Data.h"

int WsjtxTableModel::rowCount(const QModelIndex&) const {
    return wsjtxData.count();
}

int WsjtxTableModel::columnCount(const QModelIndex&) const {
    return 5;
}

QVariant WsjtxTableModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        WsjtxEntry entry = wsjtxData.at(index.row());
        switch (index.column()) {
        case 0: return QString::number(entry.decode.snr);
        case 1: return entry.callsign;
        case 2: return entry.grid;
        case 3: return entry.dxcc.country;
        case 4:
            switch (entry.status) {
            case DxccStatus::NewEntity:
                return tr("New Entity");
            case DxccStatus::NewBand:
                return tr("New Band");
            case DxccStatus::NewMode:
                return tr("New Mode");
            case DxccStatus::NewBandMode:
                return tr("New Band & Mode");
            case DxccStatus::NewSlot:
                return tr("New Slot");
            default:
                return QVariant();
            }
        default: return QVariant();
        }
    }
    else if (index.column() == 1 && role == Qt::BackgroundRole) {
        WsjtxEntry entry = wsjtxData.at(index.row());
        return Data::statusToColor(entry.status, QColor(Qt::white));
    }
    else if (index.column() == 1 && role == Qt::TextColorRole) {
        WsjtxEntry entry = wsjtxData.at(index.row());
        return Data::statusToInverseColor(entry.status, QColor(Qt::black));
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
    case 4: return tr("DXCC Status");
    default: return QVariant();
    }
}

void WsjtxTableModel::addEntry(WsjtxEntry entry) {
    beginInsertRows(QModelIndex(), wsjtxData.count(), wsjtxData.count());
    wsjtxData.append(entry);
    endInsertRows();
}


WsjtxEntry WsjtxTableModel::entry(const QModelIndex& index) {
    return wsjtxData.at(index.row());
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
        QRegExp cqRegExp("^CQ (DX |TEST )?([A-Z0-9\/]+) ?([A-Z]{2}[0-9]{2})?");
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
        band = Data::instance()->band(newStatus.dial_freq/1e6).name;
        ui->freqLabel->setText(QString("%1 MHz").arg(newStatus.dial_freq/1e6));
    }

    if (status.decoding != newStatus.decoding) {
        if (newStatus.decoding) {
            wsjtxTableModel->clear();
        }
        else {
            wsjtxTableModel->sort(1, Qt::DescendingOrder);
            if (ui->autoCallCheckBox->isChecked() && !status.tx_enabled && !status.transmitting) {
                WsjtxEntry best;
                best.status = DxccStatus::Worked;

                for (int i = 0; i < wsjtxTableModel->rowCount(); i++) {
                    WsjtxEntry entry = wsjtxTableModel->entry(wsjtxTableModel->index(i, 0));
                    if (entry.status < best.status && entry.decode.snr >= -10) {
                        best = entry;
                    }
                }

                if (!best.callsign.isEmpty()) {
                    ui->autoCallCheckBox->setCheckState(Qt::Unchecked);
                    emit reply(best.decode);
                }
            }
        }
    }

    if (status.transmitting) {
        ui->txStatus->setText(tr("Transmitting"));
    }
    else {
        ui->txStatus->setText(tr("Monitoring"));
    }

    status = newStatus;

    ui->modeLabel->setText(status.mode);
}


void WsjtxWidget::startReply(QModelIndex index) {
    WsjtxEntry entry = wsjtxTableModel->entry(index);
    emit reply(entry.decode);
}

WsjtxWidget::~WsjtxWidget()
{
    delete ui;
}
