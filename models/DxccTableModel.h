#ifndef DXCCTABLEMODEL_H
#define DXCCTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>

class DxccTableModel : public QSqlQueryModel
{
public:
    explicit DxccTableModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
};

#endif // DXCCTABLEMODEL_H
