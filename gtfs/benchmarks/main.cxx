//
// Created by MichaelBrunner on 07/06/2024.
//

#include "strategies/GtfsCalendarDateReader.h"


#include <vector>
#include <functional>
#include <iostream>
#include <src/GtfsReader.h>

int main() {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::string basePath = TEST_DATA_DIR_B;
  const std::function calendarDateStrategy = gtfs::GtfsCalendarDateReader(basePath + "calendar_dates.txt");
  strategy.push_back(calendarDateStrategy);
  const std::unique_ptr<DataReader<gtfs::GtfsData>> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const auto result = reader->getData();
  std::cout << "done reading " << result.calendarDates.size() << " objects" << std::endl;
}