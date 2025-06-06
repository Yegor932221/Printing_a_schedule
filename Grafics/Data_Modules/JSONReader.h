#ifndef JSONREADER_H
#define JSONREADER_H
#include "IDataReader.h"

class JSONReader:public IDataReader
{
public:

    QList<dataPoint> DataRead(const QString& source) override;
};

#endif // JSONREADER_H
