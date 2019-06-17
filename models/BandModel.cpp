#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include "BandModel.h"

BandModel::BandModel(bool allowEmpty, QObject *parent)
    : QAbstractListModel(parent),
      allowEmpty(allowEmpty)
{
    QFile file(":/res/data/bands.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();
    bands = QJsonDocument::fromJson(data).toVariant().toList();
}

QVariant BandModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

int BandModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return bands.size() + (allowEmpty ? 1 : 0);
}

QVariant BandModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole and index.column() == 0) {
        if (allowEmpty && index.row() == 0) {
            return "Band";
        }
        else {
            return bands.at(index.row() - (allowEmpty ? 1 : 0)).toMap()["name"];
        }
    }
    else if (role == Qt::UserRole and index.column() == 0) {
        return index.row() - (allowEmpty ? 1 : 0);
    }
    else {
        return QVariant();
    }
}
