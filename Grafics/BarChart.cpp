#include "BarChart.h"
#include <QtCharts>
#include <QDateTimeAxis>
#include <QBarCategoryAxis>
#include <QValueAxis>

void BarChart::Draw(QList<dataPoint> data, QtCharts::QChartView* chartView)
{
    // 1. Создаем набор данных
    QtCharts::QBarSeries* series = new QtCharts::QBarSeries();
    QtCharts::QBarSet* barSet = new QtCharts::QBarSet("Values");

    // 2. Агрегируем данные по дням/месяцам (для больших наборов)
    QMap<QString, double> aggregatedData;
    for (const auto& point : data) {
        QString key = point.m_date.toString("yyyy-MM-dd"); // Или "yyyy-MM" для месячной агрегации
        aggregatedData[key] += point.m_value;
    }

    // 3. Заполняем данные и категории
    QStringList categories;
    for (auto it = aggregatedData.begin(); it != aggregatedData.end(); ++it) {
        *barSet << it.value();
        categories << it.key();
    }

    series->append(barSet);

    // 4. Настраиваем график
    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Bar Chart with Aggregated Data");

    // 5. Настраиваем оси
    QtCharts::QBarCategoryAxis* axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QtCharts::QValueAxis* axisY = new QtCharts::QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 6. Оптимизации для больших данных
    if (categories.size() > 100) {
        axisX->setLabelsVisible(false); // Скрываем подписи при большом количестве
        chart->legend()->setVisible(false);
    }

    // 7. Устанавливаем график
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}
