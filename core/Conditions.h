#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class Conditions : public QObject
{
    Q_OBJECT
public:
    explicit Conditions(QObject *parent = nullptr);
    ~Conditions();

signals:
    void conditionsUpdated();

public slots:
    void update();
    void processReply(QNetworkReply* reply);

public:
    int flux;

private:
    QNetworkAccessManager* nam;
};

#endif // CONDITIONS_H
