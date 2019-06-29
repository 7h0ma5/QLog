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

    void update();

public slots:
    void loadData();
    void processReply(QNetworkReply* reply);

signals:
    void progress(int count);
    void finished(bool result);

private:
    void download();
    void parseData(QTextStream& data);

    QNetworkAccessManager* nam;
};

#endif // CTY_H
