#ifndef ICHART_H
#define ICHART_H
#include<QList>
#include "Data_Modules/IDataReader.h"
#include <QtCharts/QChartView>

class IChart{
public:
    virtual ~IChart()=default;
    virtual void Draw(QList<dataPoint> data, QtCharts::QChartView* chart, bool isMonochrome) = 0;
};

#endif // ICHART_H
