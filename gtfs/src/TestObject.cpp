//
// Created by MichaelBrunner on 27/05/2024.
//

#include "TestObject.h"

#include "DataReader.h"
#include "GtfsAgencyReader.h"
#include "GtfsReader.h"
#include <memory>
#include <LoggingPool.h>

#include <iostream>

void TestObject::testFunction() {
  std::cout << "hello world" << std::endl;
}

void testFunction() {
  std::function strategy = gtfs::GtfsAgencyReader();
  // auto currentDirectory = std::filesystem::current_path();
  // auto parentDirectory = currentDirectory.parent_path();
  const auto lFileName = R"(C:\Users\MichaelBrunner\source\master-thesis\raptorxx\gtfs\src\test-data\agency.txt)";
  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>(lFileName, std::move(strategy));
  reader->readData();
}

size_t stringLength(const char* str) {
  return strlen(str);
}
