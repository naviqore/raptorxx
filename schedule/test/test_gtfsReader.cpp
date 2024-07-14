//
// Created by MichaelBrunner on 02/06/2024.
//

#include "LoggerFactory.h"
#include "gtfs/GtfsReaderStrategyFactory.h"
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
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <fmt/core.h>

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
  getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("Service ID: {}", calendar.serviceId));
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("Start Date: {}-{}-{}", calendar.startDate.year(), calendar.startDate.month(), calendar.startDate.day()));
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("End Date: {}-{}-{}", calendar.endDate.year(), calendar.endDate.month(), calendar.endDate.day()));
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("Weekday Service: "));

    std::ranges::for_each(calendar.weekdayService, [&](const auto& dayService) {
      auto [day, service] = dayService;
      auto day_name = weekday_names[day.c_encoding()];
      getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("{}: {}", day_name, (service ? "Service" : "No service")));
    });
  });
}

void printAgency(const std::vector<schedule::gtfs::Agency>& agencies) {
  std::ranges::for_each(
    agencies, [](const auto& agency) { getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("Agency: {} {} {}", agency.agencyId, agency.name, agency.timezone)); });
}

void printCalendarDates(const std::vector<schedule::gtfs::CalendarDate>& calendarDates) {
  std::ranges::for_each(calendarDates, [](const auto& calendarDate) {
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(fmt::format("Service ID: {} Date: {}-{}-{} Exception Type: {}", calendarDate.serviceId, calendarDate.date.year(), calendarDate.date.month(), calendarDate.date.day(), static_cast<int>(calendarDate.exceptionType)));
  });
}

// TEST(TestSuiteName, TestName)
TEST(GTFS, TestStrategyReader) {
  using namespace std::literals::string_literals;
  std::string basePath = TEST_DATA_DIR;

  auto readerFactory = schedule::gtfs::GtfsReaderStrategyFactory(std::move(basePath));

  getLogger(Target::CONSOLE, LoggerName::GTFS)->setLevel(LoggerBridge::ERROR);


  // create strategy callable objects
  const auto agencyStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::AGENCY);
  const auto calendarStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR);
  const auto calendarDatesStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR_DATE);
  const auto routesStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::ROUTE);
  const auto stopStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP);
  const auto stopTimeStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP_TIME);
  const auto transferStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRANSFER);
  const auto tripStrategy = readerFactory.getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRIP);

  std::vector strategies = {agencyStrategy, calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy};

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData();
  //
  // EXPECT_FALSE(data.get().agencies.empty() || data.get().calendars.empty()
  //              || data.get().calendarDates.empty() || data.get().routes.empty() || data.get().stops.empty()
  //              || data.get().stopTimes.empty() || data.get().transfers.empty() || data.get().trips.empty());
}

TEST(GTFS, TestReaderStrategyFactory) {
  EXPECT_THROW(schedule::gtfs::GtfsReaderStrategyFactory(""), std::invalid_argument);
}
