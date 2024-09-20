#pragma once

#include "model/Agency.h"
#include "model/Calendar.h"
#include "model/CalendarDate.h"
#include "model/Route.h"
#include "model/Stop.h"
#include "model/Transfer.h"
#include "model/Trip.h"
#include "model/StopTime.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <schedule_export.h>

namespace schedule::gtfs {

  struct SCHEDULE_API GtfsData {
    std::unordered_map<std::string, std::shared_ptr<Agency>> agencies;
    std::unordered_map<std::string, std::shared_ptr<Calendar>> calendars;
    std::unordered_map<std::string, std::vector<std::shared_ptr<CalendarDate>>> calendarDates;
    std::unordered_map<std::string, std::shared_ptr<Route>> routes;
    std::unordered_map<std::string, std::shared_ptr<Stop>> stops;
    std::unordered_map<std::string, std::vector<std::shared_ptr<StopTime>>> stopTimes;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Transfer>>> transfers;
    std::unordered_map<std::string, std::shared_ptr<Trip>> trips;
    std::unordered_map<std::string, std::vector<std::string>> parentChildrenStations;

    [[nodiscard]] Agency* getAgency(const std::string& id) const;
    [[nodiscard]] Calendar* getCalendar(const std::string& id) const;
    [[nodiscard]] const std::vector<std::shared_ptr<CalendarDate>>& getCalendarDates(const std::string& id) const;
    [[nodiscard]] Route* getRoute(const std::string& id) const;
    [[nodiscard]] Stop* getStop(const std::string& id) const;
    [[nodiscard]] const std::vector<std::shared_ptr<StopTime>>& getStopTimes(const std::string& id) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Transfer>>& getTransfers(const std::string& id) const;
    [[nodiscard]] Trip* getTrip(const std::string& id) const;
    [[nodiscard]] const std::vector<std::string>& getParentChildrenStations(const std::string& id) const;
  };

} // namespace schedule::gtfs