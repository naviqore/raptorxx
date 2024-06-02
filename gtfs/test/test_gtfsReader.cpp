//
// Created by MichaelBrunner on 02/06/2024.
//

#include "utils/utils.h"
#include <DataReader.h>
#include <GtfsReader.h>
#include <strategies/GtfsAgencyReader.h>
#include <strategies/GtfsCalendarReader.h>

#include <memory>
#include <LoggingPool.h>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <gtest/gtest.h>

TEST(GTFS, TestFunction) {
  const auto agencyFile = R"(C:\Users\MichaelBrunner\source\master-thesis\raptorxx\gtfs\test\test-data\agency.txt)";
  const auto calendarFile = R"(C:\Users\MichaelBrunner\source\master-thesis\raptorxx\gtfs\test\test-data\calendar.txt)";

  const std::map<gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap = {
    {gtfs::utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
    {gtfs::utils::GTFS_FILE_TYPE::CALENDAR, calendarFile}};


  std::ignore = LoggingPool::getInstance(Target::CONSOLE);
  LoggingPool::getLogger()->set_level(spdlog::level::info);

  const std::function agencyStrategy = gtfs::GtfsAgencyReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::AGENCY));
  const std::function calendarStrategy = gtfs::GtfsCalendarReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::CALENDAR));

  std::vector strategies = {agencyStrategy, calendarStrategy};
  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData();
  std::ranges::for_each(data.agencies, [](const auto& agency) {
    LoggingPool::getLogger()->info("Agency: {} {} {}", agency.agencyId, agency.name, agency.timezone);
  });

  std::ranges::for_each(data.calendars, [&](const auto& calendar) {
    LoggingPool::getLogger()->info("Service ID: {}", calendar.serviceId);
    LoggingPool::getLogger()->info("Start Date: {}", fmt::format("{:%F}", calendar.startDate));
    LoggingPool::getLogger()->info("End Date: {}", fmt::format("{:%F}", calendar.endDate));
    LoggingPool::getLogger()->info("Weekday Service: ");
    for (const auto& [day, service] : calendar.weekdayService)
    {
      auto day_name = fmt::format("{:%A}", day);
      LoggingPool::getLogger()->info("{}: {}", day_name, (service ? "Service" : "No service"));
    }
  });
}