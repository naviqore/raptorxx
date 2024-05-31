//
// Created by MichaelBrunner on 27/05/2024.
//

#include "TestObject.h"

#include "DataReader.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategy.h"
#include <memory>
#include <LoggingPool.h>

#include <iostream>
import test.cxxmodule;

void TestObject::testFunction() {
  std::cout << "hello world" << std::endl;
  testModules();
}

void testFunction() {
  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>("file.txt", gtfs::GtfsReaderStrategy());
  reader->readData();
}