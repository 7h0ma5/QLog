#ifndef CTY_H
#define CTY_H

#include <QtCore>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

struct DxccEntity {
    QString country;
    QString prefix;
    qint32 dxcc;
    QString cont;
    qint32 cqz;
    qint32 ituz;
    float latlon[2];
    float tz;
};

struct DxccPrefix {
    QString prefix;
    bool exact;
    qint32 dxcc;
    qint32 cqz;
    qint32 ituz;
    QString cont;
    float latlon[2];
};

class Dxcc : public QObject {
    Q_OBJECT

public:
    Dxcc();
    ~Dxcc();

    DxccEntity lookup(QString callsign);

public slots:
    void processReply(QNetworkReply* reply);

private:
    void download();
    void loadData();
    void parseData(QTextStream& data);

    QNetworkAccessManager* nam;

    QHash<qint32, DxccEntity> entities;
    QHash<QString, DxccPrefix> prefixes;
};

#endif // CTY_H
