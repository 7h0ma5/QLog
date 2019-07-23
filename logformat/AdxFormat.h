#ifndef ADXFORMAT_H
#define ADXFORMAT_H

#include "LogFormat.h"

class QXmlStreamWriter;

class AdxFormat : public LogFormat {
public:
    AdxFormat(QTextStream& stream) : LogFormat(stream) {}

    void exportContact(QSqlRecord& record);
    void exportStart();
    void exportEnd();

private:
    void writeField(QString name, QString value);

    QXmlStreamWriter* writer;
};

#endif // ADIF3FORMAT_H
