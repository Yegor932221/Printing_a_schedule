#ifndef SQLREADER_H
#define SQLREADER_H
#include "IDataReader.h"
#include <QtSql>

class SQLReader:public IDataReader
{
public:
    QList<dataPoint> DataRead(const QString& source) override;
};

#endif // SQLREADER_H
