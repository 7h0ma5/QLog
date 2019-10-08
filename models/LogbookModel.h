#ifndef LOGBOOKMODEL_H
#define LOGBOOKMODEL_H

#include <QObject>
#include <QSqlTableModel>

class LogbookModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit LogbookModel(QObject* parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // LOGBOOKMODEL_H
