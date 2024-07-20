//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RouteBuilder.h"

#include "RouteBuilder.h"

namespace raptor {

  RouteBuilder::RouteBuilder(const std::string& routeId, const std::vector<std::string>& stopIds)
    : routeId_(routeId) {
    for (int i = 0; i < stopIds.size(); ++i)
    {
      stopSequence_[i] = stopIds[i];
    }
  }

  void RouteBuilder::addTrip(const std::string& tripId) {
    if (trips_.contains(tripId))
    {
      throw std::invalid_argument("Trip " + tripId + " already exists.");
    }
    trips_[tripId] = std::vector<StopTime>(stopSequence_.size());
  }

  void RouteBuilder::addStopTime(const std::string& tripId, int position, const std::string& stopId, const StopTime& stopTime) {
    if (position < 0 || position >= static_cast<int>(stopSequence_.size()))
    {
      throw std::invalid_argument("Position " + std::to_string(position) + " is out of bounds [0, " + std::to_string(stopSequence_.size()) + ").");
    }

    auto it = trips_.find(tripId);
    if (it == trips_.end())
    {
      throw std::invalid_argument("Trip " + tripId + " does not exist.");
    }

    if (stopSequence_[position] != stopId)
    {
      throw std::invalid_argument("Stop " + stopId + " does not match stop " + stopSequence_[position] + " at position " + std::to_string(position) + ".");
    }

    auto& stopTimes = it->second;
    if (stopTimes[position].departure != 0 || stopTimes[position].arrival != 0)
    {
      throw std::invalid_argument("Stop time for stop " + stopId + " already exists.");
    }

    if (position > 0)
    {
      const StopTime& previousStopTime = stopTimes[position - 1];
      if (previousStopTime.departure > stopTime.arrival)
      {
        throw std::invalid_argument("Departure time at previous stop is greater than arrival time at current stop.");
      }
    }

    if (position < static_cast<int>(stopTimes.size()) - 1)
    {
      const StopTime& nextStopTime = stopTimes[position + 1];
      if (stopTime.departure > nextStopTime.arrival && nextStopTime.arrival != 0)
      {
        throw std::invalid_argument("Departure time at current stop is greater than arrival time at next stop.");
      }
    }

    stopTimes[position] = stopTime;
  }

  void RouteBuilder::validate() {
    for (const auto& trip : trips_)
    {
      const auto& stopTimes = trip.second;
      for (const auto& stop : stopSequence_)
      {
        if (stopTimes[stop.first].arrival == 0 && stopTimes[stop.first].departure == 0)
        {
          throw std::runtime_error("Stop time at stop " + std::to_string(stop.first) + " on trip " + trip.first + " not set.");
        }
      }
    }
  }

  RouteContainer RouteBuilder::build() {
    validate();

    std::vector<std::pair<std::string, std::vector<StopTime>>> sortedEntries(trips_.begin(), trips_.end());
    std::ranges::sort(sortedEntries, [](const auto& a, const auto& b) {
      return a.second[0].departure < b.second[0].departure;
    });

    std::map<std::string, std::vector<StopTime>> sortedTrips;
    for (const auto& entry : sortedEntries)
    {
      sortedTrips[entry.first] = entry.second;
    }

    return {routeId_, stopSequence_, sortedTrips};
  }

  RouteContainer::RouteContainer(const std::string& id, const std::map<int, std::string>& stopSequence, const std::map<std::string, std::vector<StopTime>>& trips)
    : id_(id)
    , stopSequence_(stopSequence)
    , trips_(trips) {}

  bool RouteContainer::operator<(const RouteContainer& other) const {
    const StopTime& thisFirstStopTime = this->trips_.begin()->second[0];
    const StopTime& otherFirstStopTime = other.trips_.begin()->second[0];
    return thisFirstStopTime.departure < otherFirstStopTime.departure;
  }

} // namespace raptor