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

void TestObject::testFunction() {
  std::cout << "hello world" << std::endl;
}

void testFunction() {
  std::function strategy = gtfs::GtfsReaderStrategy();
  auto lambda = [](gtfs::GtfsReader const& aReader) {
    std::ignore = aReader;
    std::cout << "do something in lambda" << std::endl;
  };
  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>("file.txt", std::move(strategy));
  reader->readData();
}