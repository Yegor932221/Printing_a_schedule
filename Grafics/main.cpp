#include "mainwindow.h"
#include "AppController.h"
#include "IoCContainer.h"

// Подключаем все наши реализации
#include "Data_Modules/JSONReader.h"
#include "Data_Modules/SQLReader.h"
#include "Chart_Modules/BarChart.h"
#include "Chart_Modules/PieChart.h"
#include "Chart_Modules/SkatterChart.h"

#include <QApplication>
#include <memory>


void registerServices() {
    auto& ioc = IoCContainer::instance();

    // Регистрируем читателей
    ioc.registerService<IDataReader>("json_reader", std::make_shared<JSONReader>());
    ioc.registerService<IDataReader>("sql_reader", std::make_shared<SQLReader>());

    // Регистрируем отрисовщики графиков
    ioc.registerService<IChart>("barchart", std::make_shared<BarChart>());
    ioc.registerService<IChart>("piechart", std::make_shared<PieChart>());
    ioc.registerService<IChart>("scatterchart", std::make_shared<SkatterChart>());
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Зарегистрировать все сервисы в IoC-контейнере
    registerServices();


    AppController controller;

    MainWindow w(&controller);
    w.show();

    return a.exec();
}
