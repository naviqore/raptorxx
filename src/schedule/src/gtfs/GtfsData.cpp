//
// Created by MichaelBrunner on 19/09/2024.
//

#include "GtfsData.h"

namespace schedule::gtfs {

  Agency* GtfsData::getAgency(const std::string& id) const
  {
    return agencies.at(id).get();
  }

  Calendar* GtfsData::getCalendar(const std::string& id) const
  {
    return calendars.at(id).get();
  }

  const std::vector<std::shared_ptr<CalendarDate>>& GtfsData::getCalendarDates(const std::string& id) const
  {
    return calendarDates.at(id);
  }

  Route* GtfsData::getRoute(const std::string& id) const
  {
    return routes.at(id).get();
  }

  Stop* GtfsData::getStop(const std::string& id) const
  {
    return stops.at(id).get();
  }

  const std::vector<std::shared_ptr<StopTime>>& GtfsData::getStopTimes(const std::string& id) const
  {
    return stopTimes.at(id);
  }

  const std::vector<std::shared_ptr<Transfer>>& GtfsData::getTransfers(const std::string& id) const
  {
    return transfers.at(id);
  }

  Trip* GtfsData::getTrip(const std::string& id) const
  {
    return trips.at(id).get();
  }

  const std::vector<std::string>& GtfsData::getParentChildrenStations(const std::string& id) const
  {
    return parentChildrenStations.at(id);
  }

} // namespace schedule::gtfs
