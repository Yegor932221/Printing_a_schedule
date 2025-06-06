#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QHeaderView>
#include <QStatusBar>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QtCharts/QChartView>
#include<QLabel>

MainWindow::MainWindow(AppController* controller, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_controller(controller)
{
    ui->setupUi(this);

    setupUiLayout();
    setupConnections();


    m_controller->setChartView(m_chartView); // Сообщаем контроллеру, где рисовать

    // Устанавливаем курсор в домашнюю директорию при старте
    QString homePath = QDir::homePath();
    QModelIndex homeIndex = m_dirModel->index(homePath);
    m_treeView->setCurrentIndex(homeIndex);
    m_tableView->setRootIndex(m_fileModel->setRootPath(homePath));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiLayout() {
    this->setWindowTitle("Chart Viewer MVC/IoC");
    this->setGeometry(100, 100, 1600, 800);
    this->setStatusBar(new QStatusBar(this));

    // Модели файловой системы
    QString homePath = QDir::homePath();
    m_dirModel = new QFileSystemModel(this);
    m_dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    m_dirModel->setRootPath(homePath);

    m_fileModel = new QFileSystemModel(this);
    // Показываем только файлы .sqlite и .json
    m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    m_fileModel->setNameFilters(QStringList() << "*.json" << "*.sqlite" << "*.db");
    m_fileModel->setNameFilterDisables(false); // Не скрывать файлы, которые не соответствуют фильтру
    m_fileModel->setRootPath(homePath);

    m_treeView = new QTreeView();
    m_treeView->setModel(m_dirModel);
    m_treeView->expandToDepth(0);
    m_treeView->header()->resizeSection(0, 250);

    m_tableView = new QTableView();
    m_tableView->setModel(m_fileModel);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запретить редактирование

    QSplitter* fileBrowserSplitter = new QSplitter(Qt::Vertical);
    fileBrowserSplitter->addWidget(m_treeView);
    fileBrowserSplitter->addWidget(m_tableView);
    fileBrowserSplitter->setStretchFactor(0, 1);
    fileBrowserSplitter->setStretchFactor(1, 2);

    QWidget* chartPanel = new QWidget();
    QVBoxLayout* chartLayout = new QVBoxLayout(chartPanel);

    // Панель управления
    QWidget* controlPanel = new QWidget();
    QHBoxLayout* controlLayout = new QHBoxLayout(controlPanel);
    m_chartTypeComboBox = new QComboBox();
    m_chartTypeComboBox->addItems({"Гистограмма", "Круговая диаграмма", "Точечная диаграмма"});
    m_monochromeCheckBox = new QCheckBox("Чёрно-белый график");
    m_savePdfButton = new QPushButton("Сохранить в PDF");
    controlLayout->addWidget(new QLabel("Тип графика:"));
    controlLayout->addWidget(m_chartTypeComboBox);
    controlLayout->addWidget(m_monochromeCheckBox);
    controlLayout->addStretch();
    controlLayout->addWidget(m_savePdfButton);

    // Область для графика
    m_chartView = new QtCharts::QChartView();
    m_chartView->setRenderHint(QPainter::Antialiasing);

    chartLayout->addWidget(controlPanel);
    chartLayout->addWidget(m_chartView);
    chartPanel->setLayout(chartLayout);

    //Главный сплиттер
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(fileBrowserSplitter);
    mainSplitter->addWidget(chartPanel);
    mainSplitter->setStretchFactor(0, 1); // Левая панель
    mainSplitter->setStretchFactor(1, 3); // Правая панель

    setCentralWidget(mainSplitter);
}

void MainWindow::setupConnections() {
    // Выбор директории в дереве обновляет содержимое таблицы
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::on_directory_selectionChanged);

    // Двойной клик по файлу в таблице -> сигнал контроллеру
    connect(m_tableView, &QTableView::activated, this, &MainWindow::on_file_activated);

    // Соединяем элементы управления с контроллером
    connect(m_chartTypeComboBox, &QComboBox::currentTextChanged, m_controller, &AppController::onChartTypeChanged);
    connect(m_monochromeCheckBox, &QCheckBox::toggled, m_controller, &AppController::onMonochromeToggled);
    connect(m_savePdfButton, &QPushButton::clicked, m_controller, &AppController::onSavePdfClicked);
}

void MainWindow::on_directory_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndex index = m_treeView->selectionModel()->currentIndex();
    if (!index.isValid()) return;

    QString path = m_dirModel->filePath(index);
    this->statusBar()->showMessage("Выбранный путь: " + path);
    m_tableView->setRootIndex(m_fileModel->setRootPath(path));
}

void MainWindow::on_file_activated(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QString filePath = m_fileModel->filePath(index);
    this->statusBar()->showMessage("Загрузка файла: " + filePath);
    // Отправляем сигнал контроллеру
    m_controller->onFileSelected(filePath);
}
