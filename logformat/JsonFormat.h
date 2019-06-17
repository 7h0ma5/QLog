#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include "LogFormat.h"

class QJsonArray;

class JsonFormat : public LogFormat {
public:
    JsonFormat(QTextStream& stream) : LogFormat(stream) {}

private:
    bool importNext(QSqlRecord& contact);
    void exportContact(QSqlRecord& record);
    void exportEnd();

    QJsonArray data;
};

#endif // JSONFORMAT_H
