#include <QDebug>
#include "WsjtxWidget.h"
#include "ui_WsjtxWidget.h"
#include "core/Wsjtx.h"

WsjtxWidget::WsjtxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WsjtxWidget)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(1);
}

void WsjtxWidget::decodeReceived(WsjtxDecode decode) {
    QTableWidgetItem *newItem = new QTableWidgetItem(decode.message);
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, newItem);
    qDebug() << "Decode received";
}

void WsjtxWidget::statusReceived(WsjtxStatus status) {

}

WsjtxWidget::~WsjtxWidget()
{
    delete ui;
}
