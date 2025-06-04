#ifndef PIECHART_H
#define PIECHART_H
#include"IChart.h"
#include <QtCharts>
#include"Data_Modules/IDataReader.h"
class PieChart:public IChart
{
public:
    void Draw(QList<dataPoint> data, QtCharts::QChartView* chartView) override;
};

#endif // PIECHART_H
