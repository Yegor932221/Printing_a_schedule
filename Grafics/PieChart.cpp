
#include "PieChart.h"
#include <QtCharts>
#include <QPainter>
#include <QColor>
#include <QDateTimeAxis>

void PieChart::Draw(QList<dataPoint> data, QtCharts::QChartView* chartView)
{
    QtCharts::QPieSeries* series = new QtCharts::QPieSeries();


    for (const auto& point : data) {
        auto slice = series->append(point.m_date.toString("yyyy-MM"), point.m_value);
        qDebug() << "Slise: " << series->count();
//        slice->setColor(QColor(
//            QRandomGenerator::global()->bounded(256),
//            QRandomGenerator::global()->bounded(256),
//            QRandomGenerator::global()->bounded(256),
//            255));
        slice->setBorderColor(Qt::black);
        slice->setBorderWidth(2);
    }


    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Pie Chart"));
    series->setLabelsVisible(false);
    chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
    chart->legend()->hide();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
}
