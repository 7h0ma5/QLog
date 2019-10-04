#include <QDebug>
#include <QFile>
#include "SqlListModel.h"

SqlListModel::SqlListModel(QString query, QString placeholder, QObject *parent)
    : QSqlQueryModel(parent),
      placeholder(placeholder)
{
    this->setQuery(query);
}

QVariant SqlListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

int SqlListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return QSqlQueryModel::rowCount(parent) + (!placeholder.isEmpty() ? 1 : 0);
}

QVariant SqlListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole and index.column() == 0) {
        if (!placeholder.isEmpty() && index.row() == 0) {
            return placeholder;
        }
        else {
            QModelIndex sqlIndex = this->index(index.row() - (!placeholder.isEmpty() ? 1 : 0), index.column());
            return QSqlQueryModel::data(sqlIndex, role);
        }
    }
    else if (role == Qt::UserRole and index.column() == 0) {
        return index.row() - (placeholder.isEmpty() ? 1 : 0);
    }
    else {
        return QVariant();
    }
}
