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

    static QDate parseDate(QString date);
    static QTime parseTime(QString time);
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
    QString parseQslRcvd(QString value);
    QString parseQslSent(QString value);

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
