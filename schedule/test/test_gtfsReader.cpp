//
// Created by MichaelBrunner on 02/06/2024.
//

#include "gtfs/strategies/GtfsCalendarDateReader.h"
#include "gtfs/strategies/GtfsRouteReader.h"
#include "gtfs/strategies/GtfsStopReader.h"
#include "gtfs/strategies/GtfsStopTimeReader.h"
#include "gtfs/strategies/GtfsTransferReader.h"
#include "gtfs/strategies/GtfsTripReader.h"
#include "utils/utils.h"
#include <DataReader.h>
#include <gtfs/GtfsReader.h>
#include <gtfs/strategies/GtfsAgencyReader.h>
#include <gtfs/strategies/GtfsCalendarReader.h>

#include <memory>
#include <LoggingPool.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>

#include <gtest/gtest.h>

namespace fmt {
  template<>
  struct formatter<std::chrono::year>
  {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
      return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const std::chrono::year& y, FormatContext& ctx) {
      return fmt::format_to(ctx.out(), "{}", static_cast<int>(y));
    }
  };

  template<>
  struct formatter<std::chrono::month>
  {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
      return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const std::chrono::month& m, FormatContext& ctx) {
      return fmt::format_to(ctx.out(), "{}", static_cast<unsigned>(m));
    }
  };

  template<>
  struct formatter<std::chrono::day>
  {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
      return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const std::chrono::day& d, FormatContext& ctx) {
      return fmt::format_to(ctx.out(), "{}", static_cast<unsigned>(d));
    }
  };
}

void printCalendar(const std::vector<schedule::gtfs::Calendar>& calendars) {
  std::array<std::string, 7> weekday_names = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
  std::ranges::for_each(calendars, [&](const auto& calendar) {
    LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("Service ID: {}", calendar.serviceId));
    LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("Start Date: {}-{}-{}", calendar.startDate.year(), calendar.startDate.month(), calendar.startDate.day()));
    LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("End Date: {}-{}-{}", calendar.endDate.year(), calendar.endDate.month(), calendar.endDate.day()));
    LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("Weekday Service: "));

    std::ranges::for_each(calendar.weekdayService, [&](const auto& dayService) {
      auto [day, service] = dayService;
      auto day_name = weekday_names[day.c_encoding()];
      LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("{}: {}", day_name, (service ? "Service" : "No service")));
    });
  });
}

void printAgency(const std::vector<schedule::gtfs::Agency>& agencies) {
  std::ranges::for_each(
    agencies, [](const auto& agency) { LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("Agency: {} {} {}", agency.agencyId, agency.name, agency.timezone)); });
}

void printCalendarDates(const std::vector<schedule::gtfs::CalendarDate>& calendarDates) {
  std::ranges::for_each(calendarDates, [](const auto& calendarDate) {
    LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("Service ID: {} Date: {}-{}-{} Exception Type: {}", calendarDate.serviceId, calendarDate.date.year(), calendarDate.date.month(), calendarDate.date.day(), static_cast<int>(calendarDate.exceptionType)));
  });
}

// TEST(TestSuiteName, TestName)
TEST(GTFS, TestStrategyReader) {
  using namespace std::literals::string_literals;
  const std::string basePath = TEST_DATA_DIR;

  const auto agencyFile = basePath + "agency.txt";
  const auto calendarFile = basePath + "calendar.txt";
  const auto calendarDatesFile = basePath + "calendar_dates.txt";
  const auto routesFile = basePath + "routes.txt";
  const auto stopFile = basePath + "stops.txt";
  const auto stopTimeFile = basePath + "stop_times.txt";
  const auto TransferFile = basePath + "transfers.txt";
  const auto TripFile = basePath + "trips.txt";

  // register GTFS file names
  const std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap = {
    {schedule::gtfs::utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR, calendarFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES, calendarDatesFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::ROUTES, routesFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::STOP, stopFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES, stopTimeFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::TRANSFERS, TransferFile},
    {schedule::gtfs::utils::GTFS_FILE_TYPE::TRIPS, TripFile}};

  LoggingPool::getInstance(Target::CONSOLE)->setLevel(LoggerBridge::ERROR);


  // create strategy callable objects
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsAgencyReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::AGENCY));
  const std::function<void(gtfs::GtfsReader&)> calendarStrategy = gtfs::GtfsCalendarReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR));
  const std::function<void(gtfs::GtfsReader&)> calendarDatesStrategy = gtfs::GtfsCalendarDateReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES));
  const std::function<void(gtfs::GtfsReader&)> routesStrategy = gtfs::GtfsRouteReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::ROUTES));
  const std::function<void(gtfs::GtfsReader&)> stopStrategy = gtfs::GtfsStopReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::STOP));
  const std::function<void(gtfs::GtfsReader&)> stopTimeStrategy = gtfs::GtfsStopTimeReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES));
  const std::function<void(gtfs::GtfsReader&)> transferStrategy = gtfs::GtfsTransferReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::TRANSFERS));
  const std::function<void(gtfs::GtfsReader&)> tripStrategy = gtfs::GtfsTripReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::TRIPS));

  std::vector strategies = {agencyStrategy, calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy};

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData();

  EXPECT_FALSE(data.get().agencies.empty() || data.get().calendars.empty()
              || data.get().calendarDates.empty() || data.get().routes.empty() || data.get().stops.empty()
              || data.get().stopTimes.empty() || data.get().transfers.empty() || data.get().trips.empty());
}
