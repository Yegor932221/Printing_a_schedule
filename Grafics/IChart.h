#ifndef ICHART_H
#define ICHART_H
#include<QList>
#include "IDataReader.h"
#include <QtCharts/QChartView>

class IChart{
public:
    virtual ~IChart()=default;
    virtual void Draw(QList<dataPoint> data, QtCharts::QChartView &chart)=0;
};

#endif // ICHART_H
