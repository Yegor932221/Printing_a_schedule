#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QList>
#include "Data_Modules/IDataReader.h"
#include <QtCharts/QChartView>

// Forward declarations
class QChartView;

class AppController : public QObject {
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);

    void setChartView(QtCharts::QChartView* chartView);

public slots:
    // Слоты для реакции на действия пользователя
    void onFileSelected(const QString& filePath);
    void onChartTypeChanged(const QString& chartType);
    void onMonochromeToggled(bool isMonochrome);
    void onSavePdfClicked();

private:
    void updateChart(); // Основная функция перерисовки

    QtCharts::QChartView* m_chartView = nullptr;


    QList<dataPoint> m_currentData;
    QString m_selectedChartType = "Гистограмма";
    bool m_isMonochrome = false;
};

#endif // APPCONTROLLER_H
