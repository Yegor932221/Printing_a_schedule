#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QDebug>

#include "Data_Modules/SQLReader.h"
#include "PieChart.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Чтение данных...";
    SQLReader reader;
    //QList<dataPoint> data_base = reader.DataRead("C:/Users/Yegor/TRPO/Printing_a_schedule/InputData/HUMIDITY_MOSCOW.sqlite");
    //QList<dataPoint> data_base = reader.DataRead("C:\\Users\\Yegor\\TRPO\\Printing_a_schedule\\InputData\\BLOOD_SUGAR.sqlite");
    QList<dataPoint> data_base = reader.DataRead("C:\\Users\\Yegor\\TRPO\\Printing_a_schedule\\InputData\\NORDPOOL_PRICES.sqlite");
    //QList<dataPoint> data_base = reader.DataRead("C:\\Users\\Yegor\\TRPO\\Printing_a_schedule\\InputData\\PRICES_NATURAL_GAS_USD.sqlite");

    QtCharts::QChartView *chartView = new QtCharts::QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);

    PieChart chart;
    chart.Draw(data_base, chartView);

    // Главное окно
    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->addWidget(chartView);
    window->setLayout(layout);
    window->resize(800, 600);
    window->show();

    return app.exec();
}
