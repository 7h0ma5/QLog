#include "LogbookModel.h"
#include "data/Data.h"
#include <QIcon>

LogbookModel::LogbookModel(QObject* parent, QSqlDatabase db)
        : QSqlTableModel(parent, db)
{
    setTable("contacts");
    setEditStrategy(QSqlTableModel::OnFieldChange);
    setSort(1, Qt::DescendingOrder);

    setHeaderData(0, Qt::Horizontal, tr("ID"));
    setHeaderData(1, Qt::Horizontal, tr("Time on"));
    setHeaderData(2, Qt::Horizontal, tr("Time off"));
    setHeaderData(3, Qt::Horizontal, tr("Call"));
    setHeaderData(4, Qt::Horizontal, tr("RST Sent"));
    setHeaderData(5, Qt::Horizontal, tr("RST Rcvd"));
    setHeaderData(6, Qt::Horizontal, tr("Frequency"));
    setHeaderData(7, Qt::Horizontal, tr("Band"));
    setHeaderData(8, Qt::Horizontal, tr("Mode"));
    setHeaderData(9, Qt::Horizontal, tr("Submode"));
    setHeaderData(10, Qt::Horizontal, tr("Name"));
    setHeaderData(11, Qt::Horizontal, tr("QTH"));
    setHeaderData(12, Qt::Horizontal, tr("Gridsquare"));
    setHeaderData(13, Qt::Horizontal, tr("DXCC"));
    setHeaderData(14, Qt::Horizontal, tr("Country"));
    setHeaderData(15, Qt::Horizontal, tr("Continent"));
    setHeaderData(16, Qt::Horizontal, tr("CQ"));
    setHeaderData(17, Qt::Horizontal, tr("ITU"));
    setHeaderData(18, Qt::Horizontal, tr("Prefix"));
    setHeaderData(19, Qt::Horizontal, tr("State"));
    setHeaderData(20, Qt::Horizontal, tr("County"));
    setHeaderData(21, Qt::Horizontal, tr("IOTA"));
    setHeaderData(22, Qt::Horizontal, tr("QSL Rcvd"));
    setHeaderData(23, Qt::Horizontal, tr("QSL Rcvd Date"));
    setHeaderData(24, Qt::Horizontal, tr("QSL Sent"));
    setHeaderData(25, Qt::Horizontal, tr("QSL Sent Date"));
    setHeaderData(26, Qt::Horizontal, tr("LotW Rcvd"));
    setHeaderData(27, Qt::Horizontal, tr("LotW Rcvd Date"));
    setHeaderData(28, Qt::Horizontal, tr("LotW Sent"));
    setHeaderData(29, Qt::Horizontal, tr("LotW Sent Date"));
    setHeaderData(30, Qt::Horizontal, tr("TX Power"));
    setHeaderData(31, Qt::Horizontal, tr("Fields"));
}

QVariant LogbookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole && index.column() == 3) {
        QModelIndex dxcc_index = this->index(index.row(), 13);
        int dxcc = QSqlTableModel::data(dxcc_index, Qt::DisplayRole).toInt();
        QString flag = Data::instance()->dxccFlag(dxcc);

        if (!flag.isEmpty()) {
            return QIcon(QString(":/flags/16/%1.png").arg(flag));
        }
        else {
            return QIcon(":/flags/16/unknown.png");
        }
    }

    if (role == Qt::DecorationRole && (index.column() == 22 || index.column() == 24 ||
                                       index.column() == 26 || index.column() == 28))
    {
        QVariant value = QSqlTableModel::data(index, Qt::DisplayRole);
        if (value.toString() == "Y") {
            return QIcon(":/icons/done-24px.svg");
        }
        else {
            return QIcon(":/icons/close-24px.svg");
        }
    }
    else {
        return QSqlTableModel::data(index, role);
    }
}
