#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDataStream>
#include <QDateTime>
#include "Wsjtx.h"

Wsjtx::Wsjtx(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, 2237);

    connect(socket, &QUdpSocket::readyRead, this, &Wsjtx::readPendingDatagrams);
}

void Wsjtx::readPendingDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QDataStream stream(datagram.data());

        quint32 magic, schema, mtype;
        stream >> magic;
        stream >> schema;
        stream >> mtype;

        if (magic != 0xadbccbda) {
            qDebug() << "Invalid wsjtx magic";
            continue;
        }

        switch (mtype) {
        case 1: {
            QByteArray id, mode, tx_mode, sub_mode, report, dx_call, dx_grid, de_call, de_grid;
            WsjtxStatus status;

            stream >> id >> status.dial_freq >> mode >> dx_call >> report >> tx_mode;
            stream >> status.tx_enabled >> status.transmitting >> status.decoding;
            stream >> status.rx_df >> status.tx_df >> de_call >> de_grid >> dx_grid;
            stream >> status.tx_watchdog >> sub_mode >> status.fast_mode >> status.special_op_mode;

            status.id = QString(id);
            status.mode = QString(mode);
            status.tx_mode = QString(mode);
            status.sub_mode = QString(sub_mode);
            status.report = QString(report);
            status.dx_call = QString(dx_call);
            status.de_call = QString(de_call);
            status.de_grid = QString(de_grid);

            emit statusReceived(status);
            break;
        }
        case 2: {
            QByteArray id, mode, message;
            WsjtxDecode decode;

            stream >> id >> decode.is_new >> decode.time >> decode.snr >> decode.dt >> decode.df;
            stream >> mode >> message >> decode.low_confidence >> decode.off_air;

            decode.id = QString(id);
            decode.mode = QString(mode);
            decode.message = QString(message);

            emit decodeReceived(decode);
            break;
        }
        case 4: {
            QByteArray id, dx_call, dx_grid, mode, rprt_sent, rprt_rcvd, tx_pwr, comments;
            QByteArray name, op_call, my_call, my_grid, exch_sent, exch_rcvd;
            WsjtxLog log;

            stream >> id >> log.time_off >> dx_call >> dx_grid >> log.tx_freq >> mode >> rprt_sent;
            stream >> rprt_rcvd >> tx_pwr >> comments >> name >> log.time_on >> op_call;
            stream >> my_call >> my_grid >> exch_sent >> exch_rcvd;

            log.id = QString(id);
            log.dx_call = QString(dx_call);
            log.dx_grid = QString(dx_grid);
            log.mode = QString(mode);
            log.rprt_sent = QString(rprt_sent);
            log.rprt_rcvd = QString(rprt_rcvd);
            log.tx_pwr = QString(tx_pwr);
            log.comments = QString(comments);
            log.name = QString(name);
            log.op_call = QString(op_call);
            log.my_call = QString(my_call);
            log.my_grid = QString(my_grid);
            log.exch_sent = QString(exch_sent);
            log.exch_rcvd = QString(exch_rcvd);

            emit logReceived(log);
            break;
        }
        }
    }
}
