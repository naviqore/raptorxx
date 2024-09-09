//
// Created by MichaelBrunner on 20/07/2024.
//

#include <ranges>
#include <stdexcept>
#include <include/RouteBuilder.h>
#include <algorithm>


namespace raptor {

  RouteBuilder::RouteBuilder(const std::string& routeId, const std::vector<std::string>& stopIds)
    : routeId(routeId)
  {
    for (int i = 0; i < stopIds.size(); ++i) {
      stopSequence[i] = stopIds[i];
    }
  }

  void RouteBuilder::addTrip(const std::string& tripId)
  {
    if (trips.contains(tripId)) {
      throw std::invalid_argument("Trip " + tripId + " already exists.");
    }
    trips[tripId] = std::vector<StopTime>(stopSequence.size());
  }

  void RouteBuilder::addStopTime(const std::string& tripId, const int position, const std::string& stopId, const StopTime& stopTime)
  {
    if (position < 0 || position >= static_cast<int>(stopSequence.size())) {
      throw std::invalid_argument("Position " + std::to_string(position) + " is out of bounds [0, " + std::to_string(stopSequence.size()) + ").");
    }

    const auto it = trips.find(tripId);
    if (it == trips.end()) {
      throw std::invalid_argument("Trip " + tripId + " does not exist.");
    }

    if (stopSequence[position] != stopId) {
      throw std::invalid_argument("Stop " + stopId + " does not match stop " + stopSequence[position] + " at position " + std::to_string(position) + ".");
    }

    auto& stopTimes = it->second;
    if (stopTimes[position].departure != 0 || stopTimes[position].arrival != 0) {
      throw std::invalid_argument("Stop time for stop " + stopId + " already exists.");
    }

    if (position > 0) {
      const StopTime& previousStopTime = stopTimes[position - 1];
      if (previousStopTime.departure > stopTime.arrival) {
        throw std::invalid_argument("Departure time at previous stop is greater than arrival time at current stop.");
      }
    }

    if (position < static_cast<int>(stopTimes.size()) - 1) {
      if (const auto& [arrival, departure] = stopTimes[position + 1];
          stopTime.departure > arrival && arrival != 0) {
        throw std::invalid_argument("Departure time at current stop is greater than arrival time at next stop.");
      }
    }

    stopTimes[position] = stopTime;
  }

  RouteContainer RouteBuilder::build()
  {
    validate();

    std::vector<std::pair<std::string, std::vector<StopTime>>> sortedEntries(trips.begin(), trips.end());
    std::ranges::sort(sortedEntries, [](const auto& a, const auto& b) {
      return a.second[0].departure < b.second[0].departure;
    });

    std::map<std::string, std::vector<StopTime>> sortedTrips;
    for (const auto& [id, stopTimes] : sortedEntries) {
      sortedTrips[id] = stopTimes;
    }

    return {routeId, stopSequence, sortedTrips};
  }

  void RouteBuilder::validate() const
  {
    //TODO check if this can even be used for validation
    for (const auto& routeStopTimes : trips | std::views::values) {
      for (const auto& [key, value] : stopSequence) {
      }
    }

    // for (const auto& [tripId, routeStopTimes] : trips) {
    //   for (const auto& stopSequenceIndex : stopSequence | std::views::keys) {
    //     if (routeStopTimes[stopSequenceIndex].arrival == 0 && routeStopTimes[stopSequenceIndex].departure == 0) {
    //       throw std::runtime_error("Stop time at stop " + std::to_string(stopSequenceIndex) + " on trip " + tripId + " not set.");
    //     }
    //   }
    // }
  }

} // namespace raptor