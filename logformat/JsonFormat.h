#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include "LogFormat.h"

class QJsonArray;

class JsonFormat : public LogFormat {
public:
    JsonFormat(QTextStream& stream) : LogFormat(stream) {}

    bool importNext(QSqlRecord& contact);
    void exportContact(QSqlRecord& record);
    void exportEnd();

private:
   QJsonArray data;
};

#endif // JSONFORMAT_H
