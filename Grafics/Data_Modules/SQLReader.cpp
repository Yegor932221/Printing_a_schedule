#include "SQLReader.h"

QList<dataPoint> SQLReader::DataRead(const QString& source)
{
    QList<dataPoint> data_base;
    QSqlDatabase bd = QSqlDatabase::addDatabase("QSQLITE");
    bd.setDatabaseName(source);
    if (!bd.open()) {
        qDebug() << bd.lastError().text();
    }
    QSqlQuery query("SELECT timestamp, value FROM data_table", bd);
    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << bd.lastError().text();
    }
    while (query.next()) {
            QDateTime date = query.value("Datetime").toDateTime(); // По имени столбца
            double value = query.value("Value").toDouble();
            dataPoint point(date,value);
            if (date.isValid()) {
                data_base.push_back(point);
                qDebug() << "Datetime " << date << "Value:" << value;
            }
            else {
                qWarning() << "Найдена невалидная запись с timestamp:"
                      << query.value("Datetime").toString();
            }
    }
    return data_base;
}
