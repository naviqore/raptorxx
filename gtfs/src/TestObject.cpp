//
// Created by MichaelBrunner on 27/05/2024.
//

#include "TestObject.h"

#include "DataReader.h"
#include "GtfsAgencyReader.h"
#include "GtfsReader.h"
#include <memory>
#include <LoggingPool.h>
#include <algorithm>

#include <iostream>

void TestObject::testFunction() {
  std::cout << "hello world" << std::endl;
}

void readAgencyTestFunction(std::string const& aFileName) {
  const std::function strategy = gtfs::GtfsAgencyReader(aFileName);
  std::vector strategies = {strategy};
  // auto currentDirectory = std::filesystem::current_path();
  // auto parentDirectory = currentDirectory.parent_path();

  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData();
  std::ranges::for_each(data.agencies, [](const auto& agency) {
    std::cout << "Agency: " << agency.agencyId << " " << agency.name << " " << agency.timezone << std::endl;
  });
}

size_t stringLength(const char* str) {
  return strlen(str);
}
