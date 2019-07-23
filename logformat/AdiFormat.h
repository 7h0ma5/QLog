#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "LogFormat.h"

class AdiFormat : public LogFormat {
public:
    AdiFormat(QTextStream& stream) : LogFormat(stream) {}

    bool readContact(QVariantMap& contact);

    bool importNext(QSqlRecord& contact);
    void exportContact(QSqlRecord& record);
    void exportStart();

private:
    enum ParserState {
        START,
        FIELD,
        KEY,
        SIZE,
        DATA_TYPE,
        VALUE
    };

    void writeField(QString name, QString value, QString type="");
    void readField(QString& field, QString& value);

    QTime parseTime(QString time);

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
