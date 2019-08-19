#include <hamlib/rig.h>
#include "RigTypeModel.h"

RigTypeModel::RigTypeModel(QObject* parent)
    : QAbstractListModel(parent)
{
    rig_load_all_backends();
    rig_list_foreach(addRig, this);
    rigList.sort();
}

int RigTypeModel::rowCount(const QModelIndex&) const {
    return rigList.count();
}

QVariant RigTypeModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        return rigList.value(index.row());
    }
    return QVariant();
}

QModelIndex RigTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    int rigId = rigIds[rigList.value(row)];
    if (rigId)
        return createIndex(row, column, rigId);
    else
        return QModelIndex();
}

int RigTypeModel::addRig(const struct rig_caps* caps, void* data) {
    RigTypeModel* rigTypeModel = (RigTypeModel*)data;
    QString name = QString("%1 %2 (%3)").arg(caps->mfg_name, caps->model_name, caps->version);
    rigTypeModel->rigList.append(name);
    rigTypeModel->rigIds[name] = caps->rig_model;
    return -1;
}
