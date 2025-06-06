#include "SQLReader.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDebug>

QList<dataPoint> SQLReader::DataRead(const QString& source)
{
 QList<dataPoint> data_base;
    QString connectionName = "connection_" + QFileInfo(source).fileName();
    QSqlDatabase bd;

    if (QSqlDatabase::contains(connectionName)) {
        bd = QSqlDatabase::database(connectionName);
    } else {
        bd = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }

    bd.setDatabaseName(source);

    if (!bd.open()) {
        QString error = bd.lastError().text();
        bd.close();
        QSqlDatabase::removeDatabase(connectionName);
        throw std::runtime_error(("Ошибка открытия БД: " + error).toStdString());
    }

    QStringList tables = bd.tables();
    if (tables.isEmpty()) {
        bd.close();
        QSqlDatabase::removeDatabase(connectionName);
        throw std::runtime_error("Ошибка: в базе данных нет таблиц.");
    }

    QString tableName = tables.first();
    QSqlQuery query(bd);

    if (!query.exec(QString("SELECT Time, Value FROM %1").arg(tableName))) {
        QString error = query.lastError().text();
        bd.close();
        QSqlDatabase::removeDatabase(connectionName);
        throw std::runtime_error(("Ошибка выполнения запроса: " + error).toStdString());
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
    QSqlDatabase::removeDatabase(connectionName);

    if (data_base.isEmpty() && query.size() != 0) { // если запрос вернул строки, но мы ничего не распарсили
        throw std::runtime_error("Данные не распознаны. Проверьте имена столбцов ('Time', 'Value') и формат данных в таблице '" + tableName.toStdString() + "'.");
    }

    return data_base;
}
