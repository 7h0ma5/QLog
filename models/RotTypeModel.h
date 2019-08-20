#ifndef ROTTYPEMODEL_H
#define ROTTYPEMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class RotTypeModel : public QAbstractListModel {
    Q_OBJECT

public:
    RotTypeModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    static int addRot(const struct rot_caps* caps, void* data);

private:
    QStringList rotList;
    QMap<QString, int> rotIds;
};

#endif // RIGTYPEMODEL_H
