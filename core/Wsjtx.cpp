#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDataStream>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QDateTime>
#include "Wsjtx.h"
#include "data/Data.h"

Wsjtx::Wsjtx(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, 2237);

    connect(socket, &QUdpSocket::readyRead, this, &Wsjtx::readPendingDatagrams);
}

void Wsjtx::readPendingDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();

        wsjtxAddress = datagram.senderAddress();
        wsjtxPort = datagram.senderPort();

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

            qDebug() << decode.id << decode.message;

            emit decodeReceived(decode);
            break;
        }
        case 5: {
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

            insertContact(log);
            break;
        }
        }
    }
}

void Wsjtx::insertContact(WsjtxLog log) {
    QSqlTableModel model;
    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));

    QSqlRecord record = model.record();

    double freq = static_cast<double>(log.tx_freq)/1e6;
    QString band = Data::band(freq).name;

    record.setValue("callsign", log.dx_call);
    record.setValue("rst_rcvd", log.rprt_rcvd);
    record.setValue("rst_sent", log.rprt_sent);
    record.setValue("name", log.name);
    record.setValue("gridsquare", log.dx_grid);
    record.setValue("freq", freq);
    record.setValue("band", band);
    record.setValue("mode", log.mode);
    record.setValue("qsl_sent", "N");
    record.setValue("qsl_rcvd", "N");
    record.setValue("lotw_qsl_sent", "N");
    record.setValue("lotw_qsl_rcvd", "N");

    if (!log.tx_pwr.isEmpty()) record.setValue("tx_pwr", log.tx_pwr);

    DxccEntity dxcc = Data::instance()->lookupDxcc(log.dx_call);
    if (!dxcc.country.isEmpty()) {
        record.setValue("country", dxcc.country);
        record.setValue("cqz", dxcc.cqz);
        record.setValue("ituz", dxcc.ituz);
        record.setValue("cont", dxcc.cont);
        record.setValue("dxcc", dxcc.dxcc);
    }

    QVariantMap fields;
    if (!log.op_call.isEmpty()) fields.insert("operator", log.op_call);
    if (!log.my_grid.isEmpty()) fields.insert("my_gridsquare", log.my_grid);
    if (!log.my_call.isEmpty()) fields.insert("station_callsign", log.my_call);

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(fields));
    record.setValue("fields", QString(doc.toJson()));

    record.setValue("start_time", log.time_on);
    record.setValue("end_time", log.time_off);

    if (!model.insertRecord(-1, record)) {
        qDebug() << model.lastError();
        return;
    }

    if (!model.submitAll()) {
        qDebug() << model.lastError();
        return;
    }

    emit contactAdded(record);
}

void Wsjtx::startReply(WsjtxDecode decode) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream << static_cast<quint32>(0xadbccbda);
    stream << static_cast<quint32>(3);
    stream << static_cast<quint32>(4);
    stream << decode.id.toUtf8();
    stream << decode.time;
    stream << decode.snr;
    stream << decode.dt;
    stream << decode.df;
    stream << decode.mode.toUtf8();
    stream << decode.message.toUtf8();
    stream << decode.low_confidence;
    stream << static_cast<quint8>(0);

    socket->writeDatagram(data, wsjtxAddress, wsjtxPort);
}
