QT += core gui sql widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AppController.cpp \
    Chart_Modules/BarChart.cpp \
    Chart_Modules/PieChart.cpp \
    Chart_Modules/SkatterChart.cpp \
    Data_Modules/JSONReader.cpp \
    Data_Modules/SQLReader.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    AppController.h \
    Chart_Modules/BarChart.h \
    Chart_Modules/IChart.h \
    Chart_Modules/PieChart.h \
    Chart_Modules/SkatterChart.h \
    Data_Modules/IDataReader.h \
    Data_Modules/JSONReader.h \
    Data_Modules/SQLReader.h \
    IoCContainer.h \
    MainWindow.h \


FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
