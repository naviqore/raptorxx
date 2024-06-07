//
// Created by MichaelBrunner on 02/06/2024.
//

#include "strategies/GtfsCalendarDateReader.h"
#include "strategies/GtfsRouteReader.h"
#include "strategies/GtfsStopReader.h"
#include "strategies/GtfsStopTimeReader.h"
#include "strategies/GtfsTransferReader.h"
#include "strategies/GtfsTripReader.h"
#include "utils/utils.h"
#include <DataReader.h>
#include <GtfsReader.h>
#include <strategies/GtfsAgencyReader.h>
#include <strategies/GtfsCalendarReader.h>

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

void printCalendar(const std::vector<gtfs::Calendar>& calendars) {
  std::array<std::string, 7> weekday_names = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
  std::ranges::for_each(calendars, [&](const auto& calendar) {
    LoggingPool::getLogger()->info("Service ID: {}", calendar.serviceId);
    LoggingPool::getLogger()->info("Start Date: {}-{}-{}", calendar.startDate.year(), calendar.startDate.month(), calendar.startDate.day());
    LoggingPool::getLogger()->info("End Date: {}-{}-{}", calendar.endDate.year(), calendar.endDate.month(), calendar.endDate.day());
    LoggingPool::getLogger()->info("Weekday Service: ");

    std::ranges::for_each(calendar.weekdayService, [&](const auto& dayService) {
      auto [day, service] = dayService;
      auto day_name = weekday_names[day.c_encoding()];
      LoggingPool::getLogger()->info("{}: {}", day_name, (service ? "Service" : "No service"));
    });
  });
}

void printAgency(const std::vector<gtfs::Agency>& agencies) {
  std::ranges::for_each(agencies, [](const auto& agency) {
    LoggingPool::getLogger()->info("Agency: {} {} {}", agency.agencyId, agency.name, agency.timezone);
  });
}

void printCalendarDates(const std::vector<gtfs::CalendarDate>& calendarDates) {
  std::ranges::for_each(calendarDates, [](const auto& calendarDate) {
    LoggingPool::getLogger()->info("Service ID: {} Date: {}-{}-{} Exception Type: {}", calendarDate.serviceId, calendarDate.date.year(), calendarDate.date.month(), calendarDate.date.day(),
      static_cast<int>(calendarDate.exceptionType));
  });
}

// TEST(TestSuiteName, TestName)
TEST(GTFS, TestFunction) {
  using namespace std::literals::string_literals;
  const auto basePath = R"(C:\Users\MichaelBrunner\source\master-thesis\raptorxx\gtfs\test\test-data\)"s;

  const auto agencyFile = basePath + "agency.txt";
  const auto calendarFile = basePath + "calendar.txt";
  const auto calendarDatesFile = basePath + "calendar_dates.txt";
  const auto routesFile = basePath + "routes.txt";
  const auto stopFile = basePath + "stops.txt";
  const auto stopTimeFile = basePath + "stop_times.txt";
  const auto TransferFile = basePath + "transfers.txt";
  const auto TripFile = basePath + "trips.txt";

  // register GTFS file names
  const std::map<gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap = {
    {gtfs::utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
    {gtfs::utils::GTFS_FILE_TYPE::CALENDAR, calendarFile},
    {gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES, calendarDatesFile},
    {gtfs::utils::GTFS_FILE_TYPE::ROUTES, routesFile},
    {gtfs::utils::GTFS_FILE_TYPE::STOP, stopFile},
    {gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES, stopTimeFile},
    {gtfs::utils::GTFS_FILE_TYPE::TRANSFERS, TransferFile},
    {gtfs::utils::GTFS_FILE_TYPE::TRIPS, TripFile}};

  std::ignore = LoggingPool::getInstance(Target::CONSOLE);
  LoggingPool::getLogger()->set_level(spdlog::level::err);

  // create strategy callable objects
  const std::function agencyStrategy = gtfs::GtfsAgencyReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::AGENCY));
  const std::function calendarStrategy = gtfs::GtfsCalendarReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::CALENDAR));
  const std::function calendarDatesStrategy = gtfs::GtfsCalendarDateReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES));
  const std::function routesStrategy = gtfs::GtfsRouteReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::ROUTES));
  const std::function stopStrategy = gtfs::GtfsStopReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::STOP));
  const std::function stopTimeStrategy = gtfs::GtfsStopTimeReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES));
  const std::function transferStrategy = gtfs::GtfsTransferReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::TRANSFERS));
  const std::function tripStrategy = gtfs::GtfsTripReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::TRIPS));

  std::vector strategies = {agencyStrategy,
                            calendarStrategy,
                            calendarDatesStrategy,
                            routesStrategy,
                            stopStrategy,
                            stopTimeStrategy,
                            transferStrategy,
                            tripStrategy};

  const std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData();

  printAgency(data.agencies);
  printCalendar(data.calendars);
  printCalendarDates(data.calendarDates);

  EXPECT_EQ(1, 1);
}

