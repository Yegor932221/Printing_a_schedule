#ifndef BARCHART_H
#define BARCHART_H

#include <QtCharts>
#include <QList>
#include "Data_Modules/IDataReader.h"
#include "IChart.h"

class BarChart:public IChart
{
public:
    void Draw(QList<dataPoint> data, QtCharts::QChartView* chart, bool isMonochrome) override;
};

#endif // BARCHART_H
