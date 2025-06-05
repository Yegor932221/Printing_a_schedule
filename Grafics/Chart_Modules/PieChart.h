#ifndef PIECHART_H
#define PIECHART_H
#include"IChart.h"
#include <QtCharts>
#include"Data_Modules/IDataReader.h"
#include <QPainter>
#include <QPaintEvent>
class PieChart:public IChart
{
public:
    void Draw(QList<dataPoint> data, QtCharts::QChartView *chart) override;
};

#endif // PIECHART_H
