#include "BarChart.h"
#include <QtCharts>
#include <QDebug>
#include <QRandomGenerator>
#include <cmath>
#include <QMap>
#include <algorithm>


void BarChart::Draw(QList<dataPoint> data, QtCharts::QChartView* chartView)
{
    qDebug() << "BarChart::Draw (Grouped by Date, Time Slots in BarSets) START";

    if (!chartView) { return; }


    QList<dataPoint> validData;
    if (!data.isEmpty()) {
        for (const auto& point : data) {
            if (point.m_date.isValid() && !std::isnan(point.m_value) && !std::isinf(point.m_value)) {
                validData.append(point);
            } else {
                qWarning() << "BarChart::Draw: Skipping invalid point. Date:" << point.m_date << "Value:" << point.m_value;
            }
        }
    }
    if (validData.isEmpty()) {return;}


    try {
        //  Группировка данных по дате и затем по временным слотам

        struct AggregatedValue {
            double sum = 0; int count = 0;
            double average() const {
                return count > 0 ? sum / count : 0.0;
            }
        };

        QMap<QDate, QMap<int, AggregatedValue>> groupedData;

        QStringList slotLabels = {"00-01","01-02","02-03","03-04",
                                  "04-05","05-06","06-07","07-08",
                                  "08-09","09-10","10-11","11-12",
                                  "12-13","13-14","14-15","15-16",
                                  "16-17","17-18","18-19","19-20",
                                  "20-21", "21-22","22-23","23-00"}; // Метки для BarSets
        const int numSlots = slotLabels.size();

        double globalMinVal = (validData.isEmpty() ? 0 : validData.first().m_value);
        double globalMaxVal = (validData.isEmpty() ? 0 : validData.first().m_value);
        bool firstValFound = validData.isEmpty();


        for (const auto& point : validData) {
            if (!firstValFound) {
                globalMinVal = point.m_value;
                globalMaxVal = point.m_value;
                firstValFound = true;
            } else {
                if (point.m_value < globalMinVal) globalMinVal = point.m_value;
                if (point.m_value > globalMaxVal) globalMaxVal = point.m_value;
            }

            QDate dateKey = point.m_date.date();
            int hour = point.m_date.time().hour();
            int slotIndex = hour;
            slotIndex = qBound(0, slotIndex, numSlots - 1);

            groupedData[dateKey][slotIndex].sum += point.m_value;
            groupedData[dateKey][slotIndex].count++;
        }
        if (!firstValFound && validData.isEmpty()) { // Если validData был пуст изначально
            qWarning() << "BarChart::Draw: No valid data to display!";
            QScopedPointer<QtCharts::QChart> emptyChart(new QtCharts::QChart());
            emptyChart->setTitle("No valid data to display");
            chartView->setChart(emptyChart.take());
            return;
        }


        bool allValuesEffectivelySame = (qAbs(globalMaxVal - globalMinVal) < 1e-9);

        // Создаем серию и BarSets
        QScopedPointer<QtCharts::QBarSeries> series(new QtCharts::QBarSeries());
        QList<QtCharts::QBarSet*> barSets;
        for (const QString& label : slotLabels) {
            QtCharts::QBarSet* barSet = new QtCharts::QBarSet(label);
            barSets.append(barSet);
        }

        QStringList categories; // Даты для оси X

        // Заполняем BarSets
        for (auto dateIt = groupedData.constBegin(); dateIt != groupedData.constEnd(); ++dateIt) {
            QDate currentDate = dateIt.key();
            categories << currentDate.toString("dd.MM.yyyy"); // Категория - дата

            const QMap<int, AggregatedValue>& slotsForDate = dateIt.value();
            for (int i = 0; i < numSlots; ++i) {
                if (slotsForDate.contains(i)) {
                    *(barSets[i]) << slotsForDate[i].average();
                } else {
                    *(barSets[i]) << 0;
                }
            }
        }

        // Добавляем BarSets в серию
        for (QtCharts::QBarSet* bs : barSets) {
            series->append(bs);
        }


        QList<QColor> slotColors ={Qt::red, Qt::green, Qt::blue, Qt::cyan,
            Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
            Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow, Qt::gray, Qt::lightGray,
            Qt::darkGray,   Qt::black, QColor(255, 165, 0), QColor(128, 0, 128), QColor(75, 0, 130),
            QColor(165, 42, 42),QColor(255, 192, 203), QColor(0, 128, 128),  QColor(128, 128, 0),  QColor(210, 105, 30)
        };;

        for(int i=0; i < barSets.size() && i < slotColors.size(); ++i) {
            if(barSets[i]) barSets[i]->setColor(slotColors[i]);
        }


        series->setLabelsVisible(true);
        // series->setLabelsFormat("@value");

        // Создаем график
        QScopedPointer<QtCharts::QChart> chartObject(new QtCharts::QChart());
        QtCharts::QBarSeries* seriesPtr = series.take(); // QScopedPointer отдает владение
        chartObject->addSeries(seriesPtr); // chartObject становится родителем seriesPtr
        chartObject->setTitle("Data by Date and Time Slot");


        chartObject->legend()->setVisible(true);
        chartObject->legend()->setAlignment(Qt::AlignBottom);


        //Оси
        QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis(chartObject.get());
        axisX->append(categories); // Категории - это даты "dd.MM.yyyy"
        axisX->setLabelsAngle(-45);
        chartObject->addAxis(axisX, Qt::AlignBottom);
        seriesPtr->attachAxis(axisX);

        QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis(chartObject.get());
        // настройка диапазона оси Y на основе globalMinVal, globalMaxVal, как раньше
        double yMinRange = 0.0;
        double yMaxRange = 1.0;
        if (!allValuesEffectivelySame) {
            yMinRange = globalMinVal; yMaxRange = globalMaxVal;
            double padding = (yMaxRange - yMinRange) * 0.1;
            if (qAbs(padding) < 1e-6) padding = (yMaxRange == 0.0 ? 0.1 : qAbs(yMaxRange * 0.1));
            if (qFuzzyIsNull(padding)) padding = 0.5;
            yMinRange -= padding; yMaxRange += padding;
            if (globalMinVal >= 0 && yMinRange < 0) yMinRange = 0.0;
            if (globalMaxVal <= 0 && yMaxRange > 0) yMaxRange = 0.0;
        } else {
            if (qFuzzyCompare(globalMinVal, 0.0)) { yMinRange = -1.0; yMaxRange = 1.0; }
            else { double padding = qAbs(globalMinVal * 0.1); if (padding < 1e-6) padding = 1.0; yMinRange = globalMinVal - padding; yMaxRange = globalMaxVal + padding; }
        }
        if (yMinRange >= yMaxRange - 1e-9) { yMaxRange = yMinRange + 1.0; }
        axisY->setRange(yMinRange, yMaxRange);
        axisY->setLabelFormat("%.1f");
        // ...
        chartObject->addAxis(axisY, Qt::AlignLeft);
        seriesPtr->attachAxis(axisY);

        //Отображение
        chartView->setChart(chartObject.take());

        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QtCharts::QChartView::HorizontalRubberBand);
        chartView->setDragMode(QGraphicsView::ScrollHandDrag);


        qDebug() << "BarChart::Draw: Grouped chart created.";

    } catch (const std::exception& e) {

    } catch (...) {

    }
    qDebug() << "BarChart::Draw (Grouped) END";
}
