#ifndef SKATTERCHART_H
#define SKATTERCHART_H
#include <QtCharts>
#include <QList>
#include "Data_Modules/IDataReader.h"
#include "IChart.h"

class SkatterChart:public IChart
{
public:
    void Draw(QList<dataPoint> data, QtCharts::QChartView* chartView) override;
};

#endif // SKATTERCHART_H
