//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef RAPTORSTRATEGY_H
#define RAPTORSTRATEGY_H

#include "IConnection.h"
#include "IRaptorAlgorithmStrategy.h"
#include "Raptor.h"
#include "utils/IndexedVisitedHashMap.h"
#include "utils/IndexedVisitedSet.h"
#include "utils/Label.h"

namespace raptor::strategy {
#include <vector>
#include <unordered_map>
#include <algorithm>

  struct StopTime
  {
    unsigned int arrivalTime;
    unsigned int departureTime;
  };

  struct Transfer
  {
    int targetStop;
    int transferTime;
  };

  struct StopInfo
  {
    std::vector<int> routes;         // List of routes serving the stop
    std::vector<Transfer> transfers; // Transfers available from the stop
  };

  struct TripInfo
  {
    int stopTimesOffset; // Offset in the StopTimes array
    int numStops;        // Number of stops for this trip
  };

  struct RouteInfo
  {
    std::unordered_map<std::string, TripInfo> tripInfos; // Maps tripId to TripInfo
    std::vector<int> stops;                              // Indices of stops served by the route
  };

  class RaptorUtil
  {
  private:
    std::vector<RouteInfo> Routes;
    std::vector<StopTime> StopTimes;
    std::vector<StopInfo> Stops;
    std::unordered_map<std::string, int> stopIdToIndex;
    std::vector<std::string> tripIds;

  public:
    void initialize(const schedule::gtfs::GtfsData& gtfsData) {
      for (const auto& [routeId, route] : gtfsData.routes)
      {
        RouteInfo routeInfo;

        for (const auto& stop : route.stops)
        {
          int stopIndex = static_cast<int>(Stops.size());
          StopInfo stopInfo{};

          stopIdToIndex[stop.stopId] = stopIndex;
        }
        for (const auto& trip : route.trips)
        {
          tripIds.push_back(trip.tripId);

          TripInfo tripInfo{};
          tripInfo.stopTimesOffset = static_cast<int>(StopTimes.size());
          tripInfo.numStops = 0;

          for (const auto& stopTime : trip.stopTimes)
          {
            StopTimes.emplace_back(stopTime.arrivalTime.toSeconds(), stopTime.departureTime.toSeconds());
            tripInfo.numStops++;
          }

          routeInfo.tripInfos[trip.tripId] = tripInfo;
        }

        Routes.push_back(routeInfo);
      }
    }

    auto getTripIndex(const std::string& tripId) {
      auto it = std::ranges::find(tripIds, tripId);
      if (it != tripIds.end())
      {
        return static_cast<int>(std::distance(tripIds.begin(), it));
      }
      else
      {
        return -1;
      }
    }

    TripInfo getTripInfoByIndex(const int routeIndex, const int tripIndex) {
      const std::string& tripId = tripIds[tripIndex];
      return Routes[routeIndex].tripInfos[tripId];
    }

    std::vector<StopTime> getTripStopTimes(const int routeId, const std::string& tripId) {
      std::vector<StopTime> times;
      const auto& tripInfo = Routes[routeId].tripInfos[tripId];
      for (int i = 0; i < tripInfo.numStops; ++i)
      {
        times.push_back(StopTimes[tripInfo.stopTimesOffset + i]);
      }
      return times;
    }

    std::vector<int> getStopRoutes(int stopId) {
      return Stops[stopId].routes;
    }

    std::vector<Transfer> getStopTransfers(int stopId) {
      return Stops[stopId].transfers;
    }

    int getStopIndexFromId(const std::string& stopId) {
      return stopIdToIndex[stopId];
    }
  };



  using ParetoLabel = std::tuple<unsigned int, int>; // (arrival time, number of transfers)

  class RaptorStrategy final : public IRaptorAlgorithmStrategy
  {
    struct DepartureLabel
    {
      int depTime;
      std::string stopId;
    };

    struct StopEvent
    {
      unsigned int departureTime;
      unsigned int arrivalTime;
    };

    schedule::gtfs::TimetableManager gtfsData;
    static constexpr int MAX_ROUNDS = 5;
    std::string source;
    std::string target;
    int earliestDepartureTime{0};
    int sourceLatestDepartureTime{0};
    std::vector<DepartureLabel> collectedDepTimes;
    std::vector<int> earliestArrival;
    std::vector<int> previousRoundArrival;
    std::unordered_map<std::string, int> stopIdIndexMap;
    std::vector<std::string> allStops;
    std::unordered_map<std::string, int> routeIdIndexMap;
    std::vector<std::string> allRoutes;
    std::vector<std::vector<utils::LabelEarliestArrival>> rounds;

    utils::IndexedVisitedSet stopsUpdated;
    utils::IndexedVisitedSet stopsReached;
    utils::IndexedVisitedHashMap routesServingUpdatedStops;
    std::vector<std::vector<StopEvent>> stopEventsOfTrip;

    std::vector<int> bestTimes;

    RaptorUtil raptorUtil;


  public:
    explicit RaptorStrategy(schedule::gtfs::TimetableManager&& relationManager);

    RaptorStrategy(const RaptorStrategy&) = delete;
    RaptorStrategy& operator=(const RaptorStrategy&) = delete;
    RaptorStrategy(RaptorStrategy&&) noexcept = default;
    RaptorStrategy& operator=(RaptorStrategy&&) = default;

    std::shared_ptr<IConnection> execute(const std::string& sourceStop, const std::string& targetStop, int departureTime) override;

  private:
    void collectDepartureTimes();
    void initialize();
    void initializeStopEventsOfTrip();
    void clear();
    bool updateArrivalTime(int stopIndex, int arrivalTime);
    void relaxTransfers();
    void startNewRound();
    void collectRoutesServingUpdatedStops();
    void scanRoutes();
    int getFirstTripOfRoute(const std::string& routeId) const;
    int getLastTripOfRoute(const std::string& routeId) const;

    int getRouteIndex(const std::string& routeId) const;
    std::vector<utils::LabelEarliestArrival>& currentRound();
  };

} // strategy
// gtfs
// raptor

#endif //RAPTORSTRATEGY_H
