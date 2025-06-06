#include "PieChart.h"
#include <QtCharts>
#include <QPainter>
#include <numeric>
#include <algorithm>
#include <cmath> // для ceil

void PieChart::Draw(QList<dataPoint> data, QtCharts::QChartView* chartView, bool isMonochrome)
{
    if (data.isEmpty() || !chartView) {
        chartView->setChart(new QtCharts::QChart());
        chartView->chart()->setTitle("Нет данных для отображения");
        return;
    }

    // Лимит на количество отображаемых сегментов
    const int TARGET_SLICE_COUNT = 2000;

    QList<dataPoint> dataToDraw;
    QString title;
    title = QString("Pie Chart for %1 value").arg(data.size());
    if (data.size() <= TARGET_SLICE_COUNT) {
        dataToDraw = data;
    } else {

        std::sort(data.begin(), data.end(), [](const dataPoint& a, const dataPoint& b) {
            return a.m_date < b.m_date;
        });

        int groupSize = static_cast<int>(std::ceil(static_cast<double>(data.size()) / TARGET_SLICE_COUNT));

        // создаем группы
        for (int i = 0; i < data.size(); i += groupSize) {
            int currentGroupSize = qMin(groupSize, data.size() - i);
            if (currentGroupSize == 0) continue;

            double sum = 0.0;
            for (int j = 0; j < currentGroupSize; ++j) {
                sum += data[i + j].m_value;
            }

            double average = sum / currentGroupSize;

            dataToDraw.append(dataPoint(data[i].m_date, average));
        }
    }

    QtCharts::QPieSeries* series = new QtCharts::QPieSeries();

    std::sort(dataToDraw.begin(), dataToDraw.end(), [](const dataPoint& a, const dataPoint& b) {
        return a.m_value > b.m_value;
    });

    for(const auto& point : dataToDraw) {
        series->append(point.m_date.toString("dd.MM.yy HH:mm"), point.m_value);
    }

    for(int i = 0; i < series->count(); ++i) {
        QtCharts::QPieSlice* slice = series->slices().at(i);
        if (isMonochrome) {
            int grayValue = 50 + ((i * 37) % 170);
            slice->setColor(QColor(grayValue, grayValue, grayValue));
            slice->setBorderColor(Qt::black);
            slice->setBorderWidth(0);
        } else {
            int r = (i * 41 + 50) % 256;
            int g = (i * 61 + 100) % 256;
            int b = (i * 97 + 150) % 256;
            slice->setColor(QColor(r, g, b));
            slice->setBorderWidth(2);
            slice->setBorderColor(Qt::black);
        }
    }

    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
    chart->legend()->setVisible(false);

    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setInteractive(true);
}
