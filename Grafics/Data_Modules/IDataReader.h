#ifndef IDATAREADER_H
#define IDATAREADER_H
#include<QList>
#include<QDateTime>

struct dataPoint
{
    QDateTime m_date;
    double m_value;
    dataPoint(QDateTime date=QDateTime::currentDateTime(), double value=0):m_date(date),m_value(value) {}
};

class IDataReader
{
public:
    virtual ~IDataReader() = default;
    virtual QList<dataPoint> DataRead(const QString& source)=0;
};

#endif // IDATAREADER_H
