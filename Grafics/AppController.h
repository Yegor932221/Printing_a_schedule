#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QList>
#include "Data_Modules/IDataReader.h" // Убедитесь, что этот путь правильный
#include <QtCharts/QChartView>

// ...

class AppController : public QObject {
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);
    void setChartView(QtCharts::QChartView* chartView);

signals:
    // Сигнал для отправки сообщения об ошибке в View
    void errorOccurred(const QString& title, const QString& message);

public slots:
    void onFileSelected(const QString& filePath);
    void onChartTypeChanged(const QString& chartType);
    void onMonochromeToggled(bool isMonochrome);
    void onSavePdfClicked();

private:
    void updateChart();

    QtCharts::QChartView* m_chartView = nullptr;
    QList<dataPoint> m_currentData;
    QString m_selectedChartType = "Гистограмма";
    bool m_isMonochrome = false;
};

#endif // APPCONTROLLER_H
