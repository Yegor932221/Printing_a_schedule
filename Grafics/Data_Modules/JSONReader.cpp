#include "JSONReader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

QList<dataPoint> JSONReader::DataRead(const QString& source)
{
    QList<dataPoint> result;

    // Открываем файл
    QFile file(source);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << file.errorString();
            return result;
    }

    // Читаем и парсим JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Ошибка парсинга JSON:" << parseError.errorString();
                                                               return result;
    }

    if (!doc.isArray()) {
        qWarning() << "JSON не содержит массив данных";
        return result;
    }

    // Обрабатываем массив данных
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& item : jsonArray) {
        if (!item.isObject()) continue;

        QJsonObject obj = item.toObject();
        dataPoint entry;

        if (obj.contains("Datetime")) {
            QJsonValue tsValue = obj["Datetime"];

            if (tsValue.isString()) {
                entry.m_date = QDateTime::fromString(tsValue.toString(), Qt::ISODate);
            } else if (tsValue.isDouble()) {
                qint64 ts = tsValue.toVariant().toLongLong();
                if (ts > 100000000000) { // Если число очень большое - вероятно миллисекунды
                    entry.m_date = QDateTime::fromMSecsSinceEpoch(ts);
                } else {
                    entry.m_date = QDateTime::fromSecsSinceEpoch(ts);
                }
            }
        }

        // Читаем значение
        if (obj.contains("Value")) {
            entry.m_value = obj["Value"].toDouble();
        }

        // Проверяем валидность и добавляем
        if (entry.m_date.isValid()) {
            result.append(entry);
        } else {
            qWarning() << "Найдена запись с невалидным timestamp";
        }
    }

    return result;
}
