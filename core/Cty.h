#ifndef CTY_H
#define CTY_H

#include <QtCore>
#include <QObject>
#include "data/Dxcc.h"

class QNetworkAccessManager;
class QNetworkReply;

class Cty : public QObject {
    Q_OBJECT

public:
    Cty();
    ~Cty();

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
