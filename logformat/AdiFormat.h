#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "LogFormat.h"

class AdiFormat : public LogFormat {
public:
    AdiFormat(QTextStream& stream) : LogFormat(stream) {}

    bool readContact(QVariantMap& contact);

private:
    enum ParserState {
        START,
        FIELD,
        KEY,
        SIZE,
        DATA_TYPE,
        VALUE
    };

    bool importNext(QSqlRecord& contact);
    void exportContact(QSqlRecord& record);
    void exportStart();

    void writeField(QString name, QString value, QString type="");
    void readField(QString& field, QString& value);

    QTime parseTime(QString time);

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
