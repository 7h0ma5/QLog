#ifndef HAMQTH_H
#define HAMQTH_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class HamQTH : public QObject {
    Q_OBJECT

public:
    explicit HamQTH(QObject *parent = 0);

signals:
    void callsignResult(const QMap<QString, QString>& data);

public slots:
    void queryCallsign(QString callsign);
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;
    QString sessionId;
    QString queuedCallsign;

    void authenticate();
};

#endif // HAMQTH_H
