//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef DATAREADER_H
#define DATAREADER_H

class DataReader
{
public:
  virtual ~DataReader() = default;
  virtual void readData() = 0;
};

#endif //DATAREADER_H
