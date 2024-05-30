//
// Created by MichaelBrunner on 27/05/2024.
//

#include "TestObject.h"

#include "DataReader.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategy.h"
#include <memory>
// #include "LoggingPool.h"

#include <iostream>

void TestObject::testFunction() {
  // LoggingPool::getInstance(Target::FILE)->getLogger()->info("Hello from Test {}", 123);
  std::cout << "hello world" << std::endl;
}

void testFunction() {
  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>("file.txt", gtfs::GtfsReaderStrategy());
  reader->readData();
}