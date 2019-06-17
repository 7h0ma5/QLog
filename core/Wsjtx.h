#ifndef WSJTX_H
#define WSJTX_H

#include <QObject>
#include <QDateTime>

class QUdpSocket;

struct WsjtxStatus {
    QString id, mode, tx_mode, sub_mode;
    QString dx_call, dx_grid, de_call, de_grid;
    QString report;
    quint64 dial_freq;
    qint32 rx_df, tx_df;
    bool tx_enabled, transmitting, decoding;
    bool tx_watchdog, fast_mode;
    quint8 special_op_mode;
};


struct WsjtxDecode {
    QString id, mode, message;
    bool is_new, low_confidence, off_air;
    QTime time;
    qint32 snr;
    quint32 df;
    double dt;
};

struct WsjtxLog {
    QString id, dx_call, dx_grid, mode, rprt_sent, rprt_rcvd;
    QString tx_pwr, comments, name, op_call, my_call, my_grid;
    QString exch_sent, exch_rcvd;
    QDateTime time_on, time_off;
    quint64 tx_freq;
};


class Wsjtx : public QObject
{
    Q_OBJECT
public:
    explicit Wsjtx(QObject *parent = nullptr);

signals:
    void statusReceived(WsjtxStatus);
    void decodeReceived(WsjtxDecode);
    void logReceived(WsjtxLog);

public slots:

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket* socket;
};

#endif // WSJTX_H
