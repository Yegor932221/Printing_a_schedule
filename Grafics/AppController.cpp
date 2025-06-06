#include "AppController.h"
#include "IoCContainer.h"
#include "Chart_Modules/IChart.h"
#include "Data_Modules/IDataReader.h"
#include <QFileInfo>
#include <QDebug>
#include <QPdfWriter>
#include <QFileDialog>
#include <QPainter>

AppController::AppController(QObject *parent) : QObject(parent) {}

void AppController::setChartView(QtCharts::QChartView* chartView) {
    m_chartView = chartView;
}

void AppController::onFileSelected(const QString& filePath) {
    if (filePath.isEmpty() || !m_chartView) {
        return;
    }
    qDebug() << "Controller: File selected" << filePath;

    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    std::string readerKey;

    if (extension == "json") {
        readerKey = "json_reader";
    } else if (extension == "sqlite" || extension == "db") {
        readerKey = "sql_reader";
    } else {
        qWarning() << "Unsupported file type:" << extension;
        m_currentData.clear();
        updateChart(); // Очищаем график
        return;
    }

    try {
        auto dataReader = IoCContainer::instance().resolve<IDataReader>(readerKey);
        m_currentData = dataReader->DataRead(filePath);
        qDebug() << "Controller: Data loaded," << m_currentData.size() << "points.";
    } catch (const std::exception& e) {
        qCritical() << "Controller: Failed to read data -" << e.what();
        m_currentData.clear();
    }

    updateChart();
}

void AppController::onChartTypeChanged(const QString& chartName) {
    qDebug() << "Controller: Chart type changed to" << chartName;
    m_selectedChartType = chartName;
    updateChart();
}

void AppController::onMonochromeToggled(bool isMonochrome) {
    qDebug() << "Controller: Monochrome mode" << (isMonochrome ? "ON" : "OFF");
    m_isMonochrome = isMonochrome;
    updateChart();
}

void AppController::updateChart() {
    if (!m_chartView) return;

    std::string chartKey;
    if (m_selectedChartType == "Гистограмма") {
        chartKey = "barchart";
    } else if (m_selectedChartType == "Круговая диаграмма") {
        chartKey = "piechart";
    } else if (m_selectedChartType == "Точечная диаграмма") {
        chartKey = "scatterchart";
    } else {
        qWarning() << "Unknown chart type:" << m_selectedChartType;
        // Очистить график
        m_chartView->setChart(new QtCharts::QChart());
        return;
    }

    try {
        auto chartDrawer = IoCContainer::instance().resolve<IChart>(chartKey);
        // Вызываем Draw с новым параметром
        chartDrawer->Draw(m_currentData, m_chartView, m_isMonochrome);
        qDebug() << "Controller: Chart updated successfully.";
    } catch (const std::exception& e) {
        qCritical() << "Controller: Failed to draw chart -" << e.what();
    }
}

void AppController::onSavePdfClicked() {
    if (!m_chartView || !m_chartView->chart()) {
        qWarning() << "No chart to save.";
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(m_chartView, "Сохранить как PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) {
        return;
    }

    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15));

    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing);

    m_chartView->render(&painter);

    painter.end();
    qDebug() << "Chart saved to" << filePath;
}
