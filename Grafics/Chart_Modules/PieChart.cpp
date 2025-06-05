#include "PieChart.h"
#include <QtCharts>
#include <QPainter>

void PieChart::Draw(QList<dataPoint> data, QtCharts::QChartView* chartView)
{
    double totalValue = std::accumulate(data.begin(), data.end(), 0.0,
                                        [](double sum, const dataPoint& p) { return sum + p.m_value; });

    QtCharts::QPieSeries* series = new QtCharts::QPieSeries();

    // Ограничиваем количество отображаемых сегментов для производительности
    double perdata=(100.0f/data.size());

    for (int i = 0; i < data.size(); i += 1) {
        const auto& point = data[i];
        double percentage = (point.m_value / totalValue) * 100;
        if(percentage<perdata) continue;
        QString label = point.m_date.toString("dd.MM.yyyy HH:mm");

        QtCharts::QPieSlice* slice = series->append(label, point.m_value);

        // Генерация цвета на основе индекса и значения
        int r = (i * 50) % 256;
        int g = (int(point.m_value) * 20) % 256;
        int b = (i * 30 + int(point.m_value)) % 256;

        slice->setColor(QColor(r, g, b, 220)); // Полупрозрачность
        slice->setBorderColor(Qt::black);
        slice->setBorderWidth(2);
    }

    // Настройка диаграммы
    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle(QString("All Data Points (%1 shown)").arg(series->count()));

    // Оптимизации производительности
    chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
    chart->legend()->setVisible(false);

    // Настройка отображения
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);

    // Автомасштабирование
    chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
    chartView->setInteractive(true);
}
