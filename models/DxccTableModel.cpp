#include <QColor>
#include <QSize>
#include <QFont>
#include <QDebug>
#include "DxccTableModel.h"

DxccTableModel::DxccTableModel(QObject* parent) : QSqlQueryModel(parent) {
}


QVariant DxccTableModel::data(const QModelIndex &index, int role) const {
    if (index.column() != 0 && role == Qt::TextAlignmentRole) {
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
    else if (index.column() != 0 && role == Qt::BackgroundRole) {
        if (this->data(index, Qt::DisplayRole).toInt() > 0) {
            return QColor(Qt::green);
        }
        else {
            return QColor(Qt::red);
        }
    }
    else if (index.column() != 0 && role == Qt::TextColorRole) {
        if (this->data(index, Qt::DisplayRole).toInt() > 0) {
            return QColor(Qt::black);
        }
        else {
            return QColor(Qt::white);
        }
    }
    else if (index.column() != 0 && role == Qt::FontRole) {
        QFont font;
        font.setBold(true);
        return font;
    }

    return QSqlQueryModel::data(index, role);
}
