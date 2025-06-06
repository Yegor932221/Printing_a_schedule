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

    QFile file(source);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(("Не удалось открыть файл: " + file.errorString()).toStdString());
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError) {
        throw std::runtime_error(("Ошибка парсинга JSON: " + parseError.errorString()).toStdString());
    }

    if (!doc.isArray()) {
        throw std::runtime_error("Ошибка формата: корневой элемент не является массивом.");
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

    if (result.isEmpty() && !jsonArray.isEmpty()) {
        throw std::runtime_error("Файл содержит записи, но ни одна из них не была распознана. Проверьте формат полей 'Datetime' и 'Value'.");
    }

    return result;
}
