#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlRecord>
#include "JsonFormat.h"

void JsonFormat::exportContact(QSqlRecord& record) {
    QJsonObject contact;
    int fieldCount = record.count();
    for (int i = 0; i < fieldCount; i++) {
        QString fieldName = record.fieldName(i);
        QVariant fieldValue = record.value(i);
        if (fieldValue.isNull()) continue;
        contact[fieldName] = QJsonValue::fromVariant(fieldValue);
    }
    data.append(contact);
}

void JsonFormat::exportEnd() {
    QJsonDocument doc(data);
    QByteArray json = doc.toJson();
    stream << json;
}

bool JsonFormat::importNext(QSqlRecord& record) {
    return false;
}
