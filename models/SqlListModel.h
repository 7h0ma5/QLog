#ifndef SQLLISTMODEL_H
#define SQLLISTMODEL_H

#include <QSqlQueryModel>

class SqlListModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit SqlListModel(QString query, QString placeholder, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QString placeholder;
};

#endif // SQLLISTMODEL_H
