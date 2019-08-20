#include <hamlib/rotator.h>
#include "RotTypeModel.h"

RotTypeModel::RotTypeModel(QObject* parent)
    : QAbstractListModel(parent)
{
    rot_load_all_backends();
    rot_list_foreach(addRot, this);
    rotList.sort();
}

int RotTypeModel::rowCount(const QModelIndex&) const {
    return rotList.count();
}

QVariant RotTypeModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        return rotList.value(index.row());
    }
    return QVariant();
}

QModelIndex RotTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    int rotId = rotIds[rotList.value(row)];
    if (rotId)
        return createIndex(row, column, rotId);
    else
        return QModelIndex();
}

int RotTypeModel::addRot(const struct rot_caps* caps, void* data) {
    RotTypeModel* rotTypeModel = (RotTypeModel*)data;
    QString name = QString("%1 %2 (%3)").arg(caps->mfg_name, caps->model_name, caps->version);
    rotTypeModel->rotList.append(name);
    rotTypeModel->rotIds[name] = caps->rot_model;
    return -1;
}
