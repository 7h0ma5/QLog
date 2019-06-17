#ifndef BANDMODEL_H
#define BANDMODEL_H

#include <QAbstractListModel>

class BandModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit BandModel(bool allowEmpty = false, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    bool allowEmpty;
    QList<QVariant> bands;
};

#endif // BANDMODEL_H
