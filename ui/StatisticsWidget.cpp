#include <QChart>
#include <QChartView>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QBarSeries>
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>
#include "StatisticsWidget.h"
#include "ui_StatisticsWidget.h"

using namespace QtCharts;

StatisticsWidget::StatisticsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsWidget)
{
    ui->setupUi(this);

    QBarSet* set = new QBarSet("Total QSOs");
    QBarCategoryAxis* axisX = new QBarCategoryAxis();

    QSqlQuery query("SELECT strftime('%Y', start_time) AS year, count(1) from contacts group by year order by year ASC");
    while (query.next()) {
        axisX->append(query.value(0).toString());
        *set << query.value(1).toInt();
    }

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    series->attachAxis(axisX);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTickCount(10);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->applyNiceNumbers();
    axisY->setLabelFormat("%d");

    chart->setTitle("Total QSOs");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    this->layout()->addWidget(chartView);
}

StatisticsWidget::~StatisticsWidget()
{
    delete ui;
}
