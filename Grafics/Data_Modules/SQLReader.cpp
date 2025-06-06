#include "SQLReader.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDebug>

QList<dataPoint> SQLReader::DataRead(const QString& source)
{
    QList<dataPoint> data_base;
    QSqlDatabase bd = QSqlDatabase::addDatabase("QSQLITE");
    bd.setDatabaseName(source);

    if (!bd.open()) {
        qDebug() << "Ошибка открытия БД:" << bd.lastError().text();
        return data_base;
    }

    // Получаем список таблиц (для диагностики)
    QStringList tables = bd.tables();
    qDebug() << "Доступные таблицы:" << tables;

    if (tables.isEmpty()) {
        qDebug() << "В базе данных нет таблиц!";
        bd.close();
        return data_base;
    }

    QString tableName = tables.first();
    QSqlQuery query(bd);

    if (!query.exec(QString("SELECT Time, Value FROM %1").arg(tableName))) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        bd.close();
        return data_base;
    }

    while (query.next())
    {
        QString timeStr = query.value("Time").toString();
        double value = query.value("Value").toDouble();

        // Обрабатываем оба формата:
        //  "26.04.2010 100" (дата + минуты)
        //  "31.12.2009 18:00" (дата + время)
        QDateTime dateTime;

        if (timeStr.contains(":")) {
            dateTime = QDateTime::fromString(timeStr, "dd.MM.yyyy HH:mm");
        } else {
            QStringList parts = timeStr.split(" ");
            if (parts.size() == 2) {
                QDate date = QDate::fromString(parts[0], "dd.MM.yyyy");
                bool ok;
                int minutes = parts[1].toInt(&ok);
                if (ok && date.isValid()) {
                    QTime time(minutes / 60, minutes % 60);
                    dateTime = QDateTime(date, time);
                }
            }
        }

        if (dateTime.isValid()) {
            data_base.append(dataPoint(dateTime, value));
//            qDebug() << "Time:" << dateTime.toString("dd.MM.yyyy HH:mm")
//                     << "Value:" << value;
        } else {
            qDebug() << "Не удалось разобрать время:" << timeStr;
        }
    }

    bd.close();
    return data_base;
}
