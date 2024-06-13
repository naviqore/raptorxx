//
// Created by MichaelBrunner on 27/05/2024.
//

#include "TestObject.h"

#include "DataReader.h"
#include "gtfs/GtfsReader.h"
#include "gtfs/strategies/GtfsAgencyReader.h"
#include "gtfs/strategies/GtfsCalendarReader.h"

#include <memory>
#include <LoggingPool.h>
#include <algorithm>
#include <chrono>
#include <iostream>

void TestObject::testFunction() {
  std::cout << "hello world" << std::endl;
}

// auto currentDirectory = std::filesystem::current_path();
// auto parentDirectory = currentDirectory.parent_path();

size_t stringLength(const char* str) {
  return strlen(str);
}
