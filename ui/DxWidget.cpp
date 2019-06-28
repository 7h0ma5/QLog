#include <QDebug>
#include <QColor>
#include <QSettings>
#include "DxWidget.h"
#include "ui_DxWidget.h"
#include "data/Data.h"

int DxTableModel::rowCount(const QModelIndex&) const {
    return dxData.count();
}

int DxTableModel::columnCount(const QModelIndex&) const {
    return 6;
}

QVariant DxTableModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        Spot spot = dxData.at(index.row());
        switch (index.column()) {
        case 0:
            return spot.time.toString();
        case 1:
            return spot.callsign;
        case 2:
            return spot.freq;
        case 3:
            return spot.spotter;
        case 4:
            return spot.comment;
        case 5:
            return spot.dxcc.country;
        default:
            return QVariant();
        }
    }
    else if (index.column() == 1 && role == Qt::BackgroundRole) {
        Spot spot = dxData.at(index.row());
        switch (spot.status) {
        case DxccStatus::NewEntity:
            return QColor(Qt::red);
        default:
            return QVariant();
        }
    }
    else if (index.column() == 1 && role == Qt::TextColorRole) {
        Spot spot = dxData.at(index.row());
        switch (spot.status) {
        case DxccStatus::NewEntity:
            return QColor(Qt::white);
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QVariant DxTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section) {
    case 0: return tr("Time");
    case 1: return tr("Callsign");
    case 2: return tr("Frequency");
    case 3: return tr("Spotter");
    case 4: return tr("Comment");
    case 5: return tr("Country");
    default: return QVariant();
    }
}

void DxTableModel::addEntry(Spot entry) {
    beginInsertRows(QModelIndex(), dxData.count(), dxData.count());
    dxData.append(entry);
    endInsertRows();
}

QString DxTableModel::getCallsign(const QModelIndex& index) {
    return dxData.at(index.row()).callsign;
}

double DxTableModel::getFrequency(const QModelIndex& index) {
    return dxData.at(index.row()).freq;
}

void DxTableModel::clear() {
    beginResetModel();
    dxData.clear();
    endResetModel();
}

DxWidget::DxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DxWidget) {

    socket = nullptr;

    ui->setupUi(this);
    dxTableModel = new DxTableModel(this);
    ui->dxTable->setModel(dxTableModel);
}

void DxWidget::toggleConnect() {
    if (socket && socket->isOpen()) {
        disconnectCluster();

    }
    else {
        connectCluster();
    }
}

void DxWidget::connectCluster() {
    QStringList server = ui->serverSelect->currentText().split(":");
    QString host = server[0];
    int port = server[1].toInt();

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));

    ui->connectButton->setEnabled(false);
    ui->connectButton->setText(tr("Connecting..."));

    ui->log->clear();
    ui->dxTable->clearSelection();
    dxTableModel->clear();
    ui->dxTable->repaint();

    socket->connectToHost(host, port);
}

void DxWidget::disconnectCluster() {
    ui->sendButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Connect"));

    socket->disconnect();
    socket->close();

    delete socket;
    socket = nullptr;
}

void DxWidget::send() {
    QByteArray data;
    data.append(ui->commandEdit->text());
    data.append("\n");

    socket->write(data);

    ui->commandEdit->clear();
}

void DxWidget::receive() {
    QSettings settings;
    QString data(socket->readAll());
    QStringList lines = data.split(QRegExp("(\a|\n|\r)+"));

    foreach (QString line, lines) {
        if (line.startsWith("login") || line.contains(QRegExp("enter your call(sign)?:"))) {
            QByteArray call = settings.value("station/callsign").toByteArray();
            call.append("\n");
            socket->write(call);
        }

        if (line.startsWith("DX")) {
            int index = 0;

            QRegExp spotterRegExp("DX DE (([A-Z]|[0-9]|\\/)+):?", Qt::CaseInsensitive);
            index = spotterRegExp.indexIn(line, index);
            QString spotter = spotterRegExp.cap(1);
            index += spotter.size();

            QRegExp freqRegExp("([0-9]+\\.[0-9]+)");
            index = freqRegExp.indexIn(line, index);
            QString freq = freqRegExp.cap(1);
            index += freq.size();

            QRegExp callRegExp("(([A-Z]|[0-9]|\\/)+)");
            index = callRegExp.indexIn(line, index);
            QString call = callRegExp.cap(1);
            index += call.size();

            QRegExp commentRegExp(" (.*) ([0-9]{4})Z");
            index = commentRegExp.indexIn(line, index);
            QString comment = commentRegExp.cap(1).trimmed();
            QString time = commentRegExp.cap(2);

            DxccEntity dxcc = Data::instance()->lookupDxcc(call);
            QString country = dxcc.country;

            Spot spot;
            spot.time = QTime::currentTime();
            spot.callsign = call;
            spot.freq = freq.toDouble() / 1000;
            spot.band = Data::band(spot.freq);
            spot.spotter = spotter;
            spot.comment = comment;
            spot.dxcc = dxcc;
            spot.status = Data::dxccStatus(spot.dxcc.dxcc, spot.band, "");

            dxTableModel->addEntry(spot);
            ui->dxTable->repaint();
        }

        ui->log->appendPlainText(line);
    }
}

void DxWidget::socketError(QAbstractSocket::SocketError) {
    ui->sendButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Connect"));
}

void DxWidget::connected() {
    ui->sendButton->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Disconnect"));
}

void DxWidget::rawModeChanged() {
    if (ui->rawCheckBox->isChecked()) {
        ui->stack->setCurrentIndex(1);
    }
    else {
        ui->stack->setCurrentIndex(0);
    }
}

void DxWidget::entryDoubleClicked(QModelIndex index) {
    QString callsign = dxTableModel->getCallsign(index);
    double frequency = dxTableModel->getFrequency(index);
    emit tuneDx(callsign, frequency);
}

DxWidget::~DxWidget() {
    delete ui;
}
