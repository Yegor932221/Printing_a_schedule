#include "SkatterChart.h"
#include <QDebug>
#include <cmath>
#include <QRandomGenerator>


void SkatterChart::Draw(QList<dataPoint> data, QtCharts::QChartView* chartView, bool isMonochrome)
{
    qDebug() << "SkatterChart::Draw START";

    // Проверка входных параметров
    if (!chartView) {
        qWarning() << "SkatterChart::Draw: ChartView is null!";
        return;
    }
    qDebug() << "SkatterChart::Draw: ChartView is valid.";


    // Фильтрация и подготовка входных данных
    QList<dataPoint> validData;
    double minX = 0.0, maxX = 0.0; // Для оси X (timestamp)
    double minY = 0.0, maxY = 0.0; // Для оси Y (value)
    bool firstValidPointFound = false;

    if (data.isEmpty()) {
        qWarning() << "SkatterChart::Draw: Input data is empty. Displaying empty chart message.";
        QScopedPointer<QtCharts::QChart> emptyChart(new QtCharts::QChart());
        if (!emptyChart) { qCritical() << "SkatterChart::Draw: Failed to create emptyChart for empty data!"; return; }
        emptyChart->setTitle("No data to display");
        chartView->setChart(emptyChart.take()); // QChartView становится родителем
        qDebug() << "SkatterChart::Draw: Displaying message chart for empty data.";
        return;
    }

    qDebug() << "SkatterChart::Draw: Processing" << data.size() << "input data points.";
    for (const auto& point : data) {
        if (!point.m_date.isValid() || std::isnan(point.m_value) || std::isinf(point.m_value)) {
            qWarning() << "SkatterChart::Draw: Skipping invalid data point. Date:" << point.m_date << "Value:" << point.m_value;
            continue;
        }

        validData.append(point);
        qint64 timestamp = point.m_date.toMSecsSinceEpoch();

        if (!firstValidPointFound) {
            minX = static_cast<double>(timestamp);
            maxX = static_cast<double>(timestamp);
            minY = point.m_value;
            maxY = point.m_value;
            firstValidPointFound = true;
        } else {
            if (timestamp < minX) minX = static_cast<double>(timestamp);
            if (timestamp > maxX) maxX = static_cast<double>(timestamp);
            if (point.m_value < minY) minY = point.m_value;
            if (point.m_value > maxY) maxY = point.m_value;
        }
    }

    if (validData.isEmpty()) {
        qWarning() << "SkatterChart::Draw: No valid data points after filtering. Displaying empty chart message.";
        QScopedPointer<QtCharts::QChart> emptyChart(new QtCharts::QChart());
        if (!emptyChart) { qCritical() << "SkatterChart::Draw: Failed to create emptyChart for no valid data!"; return; }
        emptyChart->setTitle("No valid data to display");
        chartView->setChart(emptyChart.take());
        qDebug() << "SkatterChart::Draw: Displaying message chart for no valid data.";
        return;
    }
    qDebug() << "SkatterChart::Draw: Filtered to" << validData.size() << "valid data points.";
    qDebug() << "SkatterChart::Draw: X Range (ms):" << minX << "to" << maxX;
    qDebug() << "SkatterChart::Draw: Y Range:" << minY << "to" << maxY;

    // Используем QScopedPointer для управления QChart и QScatterSeries
    QScopedPointer<QtCharts::QChart> chartObject(new QtCharts::QChart());
    if (!chartObject) {
        qCritical() << "SkatterChart::Draw: Failed to create QChart!";
        return;
    }
    qDebug() << "SkatterChart::Draw: New QChart created:" << chartObject.data();

    QScopedPointer<QtCharts::QScatterSeries> series(new QtCharts::QScatterSeries());
    if (!series) {
        qCritical() << "SkatterChart::Draw: Failed to create QScatterSeries!";
        // chartObject будет удален QScopedPointer, когда он выйдет из области видимости
        return;
    }
    qDebug() << "SkatterChart::Draw: New QScatterSeries created:" << series.data();

    try {
        //Настройка и заполнение QScatterSeries
        series->setName("Data Points");
        series->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(4.0);
        series->setColor(QColor(30, 144, 255));
        series->setBorderColor(Qt::black);
        for (const auto& point : validData) {
            series->append(static_cast<double>(point.m_date.toMSecsSinceEpoch()), point.m_value);
        }
        qDebug() << "SkatterChart::Draw: Series populated with" << series->count() << "points.";

        //Добавление серии в график и настройка графика
        QtCharts::QAbstractSeries* seriesPtr = series.take();
        chartObject->addSeries(seriesPtr);
        chartObject->setTitle("Scatter Plot of Data");
        chartObject->legend()->setVisible(true);
        chartObject->legend()->setAlignment(Qt::AlignBottom);
        qDebug() << "SkatterChart::Draw: Series added to chart.";

        //Создание и настройка осей
        QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis(chartObject.data());
        if (!axisX) { qCritical() << "SkatterChart::Draw: Failed to create QDateTimeAxis!"; return; } // chartObject и seriesPtr будут удалены
        axisX->setTickCount(10);
        axisX->setFormat("dd.MM.yyyy HH:mm:ss");
        axisX->setTitleText("Time");
        if (firstValidPointFound) {
            QDateTime minDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(minX));
            QDateTime maxDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(maxX));
            if (minDateTime == maxDateTime) {
                axisX->setRange(minDateTime.addSecs(-60), maxDateTime.addSecs(60));
            } else {
                // Небольшой отступ, если диапазон есть
                qint64 rangeMs = maxDateTime.toMSecsSinceEpoch() - minDateTime.toMSecsSinceEpoch();
                qint64 paddingMs = qMax(1000LL, rangeMs / 20); // 5% отступа или минимум 1 секунда
                axisX->setRange(minDateTime.addMSecs(-paddingMs), maxDateTime.addMSecs(paddingMs));
            }
        }
        chartObject->addAxis(axisX, Qt::AlignBottom);
        seriesPtr->attachAxis(axisX); // Привязка серии к оси
        qDebug() << "SkatterChart::Draw: X-axis (DateTime) configured.";

        // Ось Y (значение)
        QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis(chartObject.data());
        if (!axisY) { qCritical() << "SkatterChart::Draw: Failed to create QValueAxis!"; return; }
        axisY->setLabelFormat("%.2f");
        axisY->setTitleText("Value");
        if (firstValidPointFound) {
            double yRange = maxY - minY;
            double yPadding = yRange * 0.1;
            if (qAbs(yRange) < 1e-6) { // Если все значения одинаковы или очень близки
                yPadding = qMax(0.5, qAbs(minY * 0.1)); // Отступ 0.5 или 10% от значения
            }
            double finalMinY = minY - yPadding;
            double finalMaxY = maxY + yPadding;
            if (qAbs(finalMinY - finalMaxY) < 1e-9) { // Дополнительная проверка, если padding был слишком мал
                finalMinY = minY - (qAbs(minY)>1e-6 ? qAbs(minY*0.1) : 0.5); // Если minY не 0, то 10% от него, иначе 0.5
                finalMaxY = maxY + (qAbs(maxY)>1e-6 ? qAbs(maxY*0.1) : 0.5); // Аналогично
                if (qAbs(finalMinY-finalMaxY) < 1e-9) { // Если все равно схлопнулось (например, minY=maxY=0)
                    finalMinY -= 0.5;
                    finalMaxY += 0.5;
                }
            }
            axisY->setRange(finalMinY, finalMaxY);
        }
        chartObject->addAxis(axisY, Qt::AlignLeft);
        seriesPtr->attachAxis(axisY); // Привязка серии к оси
        qDebug() << "SkatterChart::Draw: Y-axis (Value) configured.";

        //Отображение графика
        chartView->setChart(chartObject.take());
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
        chartView->setDragMode(QGraphicsView::ScrollHandDrag);
        qDebug() << "SkatterChart::Draw: ChartView configured and new chart set.";

    } catch (const std::exception& e) {
        qCritical() << "SkatterChart::Draw: Exception caught:" << e.what();
    } catch (...) {
        qCritical() << "SkatterChart::Draw: Unknown exception caught!";
    }

    qDebug() << "SkatterChart::Draw END";
}
