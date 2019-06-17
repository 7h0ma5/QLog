#include <QtSql>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QStyledItemDelegate>
#include "models/BandModel.h"
#include "LogbookWidget.h"
#include "ui_LogbookWidget.h"

class CallsignDelegate : public QStyledItemDelegate {
public:
    CallsignDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
        QStyledItemDelegate::initStyleOption(option, index);
        option->font.setBold(true);
    }
};

class DateFormatDelegate : public QStyledItemDelegate {
public:
    DateFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toDate().toString(locale.dateFormat(QLocale::ShortFormat));
    }
};

class TimeFormatDelegate : public QStyledItemDelegate {
public:
    TimeFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toTime().toString(locale.timeFormat(QLocale::ShortFormat));
    }
};

class TimestampFormatDelegate : public QStyledItemDelegate {
public:
    TimestampFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toDateTime().toTimeSpec(Qt::UTC).toString(locale.dateTimeFormat(QLocale::ShortFormat));
    }
};

class UnitFormatDelegate : public QStyledItemDelegate {
public:
    UnitFormatDelegate(QString unit, int precision, double step, QObject* parent = 0) :
        QStyledItemDelegate(parent), unit(unit), precision(precision), step(step) { }

    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
        QStyledItemDelegate::initStyleOption(option, index);
        option->displayAlignment = Qt::AlignVCenter | Qt::AlignRight;
    }

    QString displayText(const QVariant& value, const QLocale&) const {
        return QString("%1 %2").arg(QString::number(value.toDouble(), 'f', 3), unit);
    }

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
        editor->setDecimals(precision);
        editor->setRange(0, 1e12);
        editor->setSingleStep(step);
        return editor;
    }

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex&) const
    {
        editor->setGeometry(option.rect);
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const
    {
        double value = index.model()->data(index, Qt::EditRole).toDouble();
        QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
        QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }

private:
    QString unit;
    int precision;
    double step;
};


LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);

    model = new QSqlTableModel(this);
    model->setTable("contacts");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setSort(1, Qt::DescendingOrder);

    model->setHeaderData(1, Qt::Horizontal, tr("Time on"));
    model->setHeaderData(2, Qt::Horizontal, tr("Time off"));
    model->setHeaderData(3, Qt::Horizontal, tr("Call"));
    model->setHeaderData(4, Qt::Horizontal, tr("RST Sent"));
    model->setHeaderData(5, Qt::Horizontal, tr("RST Rcvd"));
    model->setHeaderData(6, Qt::Horizontal, tr("Frequency"));
    model->setHeaderData(7, Qt::Horizontal, tr("Band"));
    model->setHeaderData(8, Qt::Horizontal, tr("Mode"));
    model->setHeaderData(9, Qt::Horizontal, tr("Submode"));
    model->setHeaderData(10, Qt::Horizontal, tr("Name"));
    model->setHeaderData(11, Qt::Horizontal, tr("QTH"));
    model->setHeaderData(12, Qt::Horizontal, tr("Gridsquare"));
    model->setHeaderData(13, Qt::Horizontal, tr("DXCC"));
    model->setHeaderData(14, Qt::Horizontal, tr("Country"));
    model->setHeaderData(15, Qt::Horizontal, tr("Continent"));
    model->setHeaderData(16, Qt::Horizontal, tr("CQ"));
    model->setHeaderData(17, Qt::Horizontal, tr("ITU"));
    model->setHeaderData(18, Qt::Horizontal, tr("Prefix"));

    /*
    QSettings settings;
    QByteArray logbookState = settings.value("logbook/state").toByteArray();
    ui->contactTable->horizontalHeader()->restoreState(logbookState);
    */

    ui->contactTable->setModel(model);

    ui->contactTable->addAction(ui->deleteContact);
    //ui->contactTable->sortByColumn(1, Qt::DescendingOrder);

    ui->contactTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->contactTable->horizontalHeader()->setSectionsMovable(true);

    ui->contactTable->setItemDelegateForColumn(1, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(2, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(3, new CallsignDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(6, new UnitFormatDelegate("MHz", 6, 0.001, ui->contactTable));

    ui->contactTable->hideColumn(0);
    ui->contactTable->hideColumn(2);
    ui->contactTable->hideColumn(9);
    ui->contactTable->hideColumn(13);
    ui->contactTable->hideColumn(15);
    ui->contactTable->hideColumn(18);
    ui->contactTable->hideColumn(19);

    model->select();

    ui->bandFilter->setModel(new BandModel(true));
}

void LogbookWidget::callsignFilterChanged() {
    QString callsign = ui->callsignFilter->text();
    if (!callsign.isEmpty()) {
        model->setFilter(QString("callsign LIKE '%%1%'").arg(ui->callsignFilter->text()));
        qDebug() << model->filter();
    }
    else {
        model->setFilter(nullptr);
    }
    qDebug() << "Callsign filter changed";
}

void LogbookWidget::bandFilterChanged() {
    QString band = ui->bandFilter->currentText();
    if (ui->bandFilter->currentIndex() != 0 && !band.isEmpty()) {
        model->setFilter(QString("band = '%1'").arg(band));
    }
    else {
        model->setFilter(nullptr);
    }
    qDebug() << "Band filter changed";
}

void LogbookWidget::deleteContact() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete"), tr("Delete the selected contacts?"),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        model->removeRow(index.row());
    }
    ui->contactTable->clearSelection();
    model->select();
}

void LogbookWidget::updateTable() {
    model->select();
}

LogbookWidget::~LogbookWidget() {
    QSettings settings;
    QByteArray logbookState = ui->contactTable->horizontalHeader()->saveState();
    settings.setValue("logbook/state", logbookState);
    delete ui;
}
