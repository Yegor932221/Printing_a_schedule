#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include "AppController.h"

// Forward declarations
namespace Ui { class MainWindow; }
class QTreeView;
class QTableView;
class QItemSelection;
class QComboBox;
class QCheckBox;
class QPushButton;
namespace QtCharts { class QChartView; }


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Передаем контроллер в конструктор
    explicit MainWindow(AppController* controller, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слот для выбора папки в дереве
    void on_directory_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    // Слот для выбора файла в таблице
    void on_file_activated(const QModelIndex &index);

private:
    void setupUiLayout(); // Метод для создания интерфейса
    void setupConnections(); // Метод для установки соединений

    Ui::MainWindow *ui;
    AppController* m_controller; // Указатель на контроллер

    // Элементы интерфейса
    QFileSystemModel *m_dirModel;
    QFileSystemModel *m_fileModel;
    QTreeView *m_treeView;
    QTableView *m_tableView;
    QtCharts::QChartView* m_chartView;
    QComboBox* m_chartTypeComboBox;
    QCheckBox* m_monochromeCheckBox;
    QPushButton* m_savePdfButton;
};
#endif // MAINWINDOW_H
