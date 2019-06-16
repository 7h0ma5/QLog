#ifndef ADXFORMAT_H
#define ADXFORMAT_H

#include "logformat.h"

class QXmlStreamWriter;

class AdxFormat : public LogFormat {
public:
    AdxFormat(QTextStream& stream) : LogFormat(stream) {}

private:
    void exportContact(QSqlRecord& record);
    void exportStart();
    void exportEnd();

    void writeField(QString name, QString value);

    QXmlStreamWriter* writer;
};

#endif // ADIF3FORMAT_H
