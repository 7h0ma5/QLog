#ifndef FLDIGI_H
#define FLDIGI_H

#include <QTcpServer>

class QXmlStreamReader;

class Fldigi : public QTcpServer {
    Q_OBJECT

public:
    explicit Fldigi(QObject *parent = 0);
    void incomingConnection(int socket);

signals:
    void contactAdded();

public slots:
    void readClient();
    void discardClient();

private:
    void processMethodCall(QTcpSocket* sock, QXmlStreamReader& xml);
    QString parseParam(QXmlStreamReader& xml);
    QByteArray listMethods();
    QByteArray addRecord(QString data);
};

#endif // FLDIGI_H
