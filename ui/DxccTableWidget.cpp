#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>
#include "models/DxccTableModel.h"
#include "DxccTableWidget.h"

DxccTableWidget::DxccTableWidget(QWidget *parent) : QTableView(parent) {
    dxccTableModel = new DxccTableModel;

    this->setObjectName("dxccTableView");
    this->setModel(dxccTableModel);
    this->verticalHeader()->setVisible(false);
    this->verticalHeader()->setDefaultSectionSize(16);
    //this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    this->setGridStyle(Qt::PenStyle::NoPen);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DxccTableWidget::clear() {
    dxccTableModel->clear();
    this->show();
}

void DxccTableWidget::setDxcc(int dxcc) {
    if (dxcc)  {
        dxccTableModel->setQuery(QString(
                             "SELECT bands.name,\n"
                             "count(CASE WHEN modes.dxcc = 'CW' THEN 1 END) as cw,\n"
                             "count(CASE WHEN modes.dxcc = 'PHONE' THEN 1 END) as phone,\n"
                             "count(CASE WHEN modes.dxcc = 'DIGITAL' THEN 1 END) as digital\n"
                             "FROM contacts\n"
                             "INNER JOIN modes ON (contacts.dxcc = %1 AND contacts.mode = modes.name)\n"
                             "RIGHT JOIN bands ON (contacts.band = bands.name)\n"
                             "WHERE bands.enabled = true\n"
                             "GROUP BY bands.name, bands.start_freq\n"
                             "ORDER BY bands.start_freq").arg(dxcc));

        dxccTableModel->setHeaderData(0, Qt::Horizontal, tr("Band"));
        dxccTableModel->setHeaderData(1, Qt::Horizontal, tr("CW"));
        dxccTableModel->setHeaderData(2, Qt::Horizontal, tr("PH"));
        dxccTableModel->setHeaderData(3, Qt::Horizontal, tr("DIG"));

        this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        this->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        this->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    }
    else {
        dxccTableModel->clear();
    }

    this->show();
}
