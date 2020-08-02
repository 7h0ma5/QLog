#include <QtSql>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QStyledItemDelegate>
#include <QDesktopServices>
#include <QMenu>
#include "logformat/AdiFormat.h"
#include "models/LogbookModel.h"
#include "models/SqlListModel.h"
#include "core/ClubLog.h"
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

    model = new LogbookModel(this);
    ui->contactTable->setModel(model);

    ui->contactTable->addAction(ui->actionFilter);
    ui->contactTable->addAction(ui->actionLookup);
    ui->contactTable->addAction(ui->actionUploadClublog);
    ui->contactTable->addAction(ui->actionDeleteContact);
    //ui->contactTable->sortByColumn(1, Qt::DescendingOrder);

    ui->contactTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->contactTable->horizontalHeader(), &QHeaderView::customContextMenuRequested,
            this, &LogbookWidget::showTableHeaderContextMenu);

    ui->contactTable->setItemDelegateForColumn(1, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(2, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(3, new CallsignDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(6, new UnitFormatDelegate("MHz", 6, 0.001, ui->contactTable));

    QSettings settings;
    QVariant logbookState = settings.value("logbook/state");
    if (!logbookState.isNull()) {
        ui->contactTable->horizontalHeader()->restoreState(logbookState.toByteArray());
    }
    else {
        ui->contactTable->hideColumn(0);
        ui->contactTable->hideColumn(2);
        ui->contactTable->hideColumn(9);
        ui->contactTable->hideColumn(13);
        ui->contactTable->hideColumn(15);
        ui->contactTable->hideColumn(18);
        ui->contactTable->hideColumn(19);
        ui->contactTable->hideColumn(24);
        ui->contactTable->hideColumn(26);
        ui->contactTable->hideColumn(28);
        ui->contactTable->hideColumn(30);
    }

    ui->contactTable->horizontalHeader()->setSectionsMovable(true);

    ui->bandFilter->setModel(new SqlListModel("SELECT name FROM bands", "Band"));
    ui->modeFilter->setModel(new SqlListModel("SELECT name FROM modes", "Mode"));
    ui->countryFilter->setModel(new SqlListModel("SELECT name FROM dxcc_entities ORDER BY name", "Country"));

    clublog = new ClubLog(this);

    updateTable();
}

void LogbookWidget::filterSelectedCallsign() {
    QModelIndex index = ui->contactTable->selectionModel()->selectedRows().first();
    QSqlRecord record = model->record(index.row());
    filterCallsign(record.value("callsign").toString());
}

void LogbookWidget::lookupSelectedCallsign() {
    QModelIndex index = ui->contactTable->selectionModel()->selectedRows().first();
    QSqlRecord record = model->record(index.row());
    QString callsign = record.value("callsign").toString();
    QDesktopServices::openUrl(QString("https://www.qrz.com/lookup/%1").arg(callsign));
}

void LogbookWidget::filterCallsign(QString call) {
    ui->callsignFilter->setText(call);
}

void LogbookWidget::callsignFilterChanged() {
    QString callsign = ui->callsignFilter->text();
    if (!callsign.isEmpty()) {
        model->setFilter(QString("callsign LIKE '%1%'").arg(ui->callsignFilter->text()));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::bandFilterChanged() {
    QString band = ui->bandFilter->currentText();
    if (ui->bandFilter->currentIndex() != 0 && !band.isEmpty()) {
        model->setFilter(QString("band = '%1'").arg(band));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::modeFilterChanged() {
    QString mode = ui->modeFilter->currentText();
    if (ui->modeFilter->currentIndex() != 0 && !mode.isEmpty()) {
        model->setFilter(QString("mode = '%1'").arg(mode));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::countryFilterChanged() {
    QString country = ui->countryFilter->currentText();
    if (ui->countryFilter->currentIndex() != 0 && !country.isEmpty()) {
        model->setFilter(QString("country = '%1'").arg(country));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::uploadClublog() {
    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    AdiFormat adi(stream);

    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        QSqlRecord record = model->record(index.row());
        adi.exportContact(record);
    }

    stream.flush();

    clublog->uploadAdif(data);
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
    updateTable();
}

void LogbookWidget::updateTable() {
    model->select();
    ui->contactTable->resizeColumnsToContents();
}

void LogbookWidget::saveTableHeaderState() {
    QSettings settings;
    QByteArray logbookState = ui->contactTable->horizontalHeader()->saveState();
    settings.setValue("logbook/state", logbookState);
}

void LogbookWidget::showTableHeaderContextMenu(const QPoint& point) {
    QMenu* contextMenu = new QMenu(this);
    for (int i = 0; i < model->columnCount(); i++) {
        QString name = model->headerData(i, Qt::Horizontal).toString();
        QAction* action = new QAction(name, contextMenu);
        action->setCheckable(true);
        action->setChecked(!ui->contactTable->isColumnHidden(i));

        connect(action, &QAction::triggered, [this, i]() {
            ui->contactTable->setColumnHidden(i, !ui->contactTable->isColumnHidden(i));
            saveTableHeaderState();
        });

        contextMenu->addAction(action);
    }
    contextMenu->exec(point);
}

LogbookWidget::~LogbookWidget() {
    saveTableHeaderState();
    delete ui;
}
