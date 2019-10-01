#ifndef CLUBLOG_H
#define CLUBLOG_H

#include <QObject>
#include <QSqlRecord>

class QNetworkReply;
class QNetworkAccessManager;

class ClubLog : public QObject
{
    Q_OBJECT
public:
    explicit ClubLog(QObject *parent = nullptr);

signals:

public slots:
    void uploadContact(QSqlRecord record);
    void uploadAdif(QByteArray& data);
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;
};

#endif // CLUBLOG_H
