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

  using ParetoLabel = std::tuple<unsigned int, int>; // (arrival time, number of transfers)

  class RaptorStrategy final : public IRaptorAlgorithmStrategy
  {
    schedule::gtfs::RelationManager relationManager;
    static constexpr int MAX_ROUNDS = 5;
    [[nodiscard]] std::unordered_map<std::string, std::set<utils::Label>> initializeStopLabels(unsigned int sourceDepartureTime, std::vector<std::string> const& departureStopIds) const;
    std::vector<std::tuple<std::string, unsigned int, std::string, std::string>> reconstructConnections(const std::unordered_map<std::string, std::set<utils::Label>>& stopLabels, const std::vector<std::string>& arrivalStopIds);
    [[nodiscard]] size_t getTotalNumberOfStops() const;
    [[nodiscard]] std::unordered_map<std::string, int> createStopIndexMap() const;
    [[nodiscard]] std::vector<schedule::gtfs::StopTime> stopTimesForStopIdAndTimeGreaterThan(std::string const& stopId, unsigned int departureTime) const;

  public:
    explicit RaptorStrategy(schedule::gtfs::RelationManager&& relationManager);

    RaptorStrategy(const RaptorStrategy&) = delete;
    RaptorStrategy& operator=(const RaptorStrategy&) = delete;
    RaptorStrategy(RaptorStrategy&&) noexcept = default;
    RaptorStrategy& operator=(RaptorStrategy&&) = default;

   std::shared_ptr<IConnection> execute(const utils::ConnectionRequest& request) override;

  private:
    struct StopEvent {
      int depTime, arrTime;
      StopEvent(const int depTime, const int arrTime) : depTime(depTime), arrTime(arrTime) {}
    };

    std::vector<int> stops;
    std::vector<std::string> stopNames;
    std::vector<std::vector<StopEvent>> stopEventsOfTrip;
    std::vector<int> trips;
    std::vector<int> firstTripOfRoute;
    std::vector<schedule::gtfs::Transfer> transfers;
    std::unordered_map<std::string, int> stopMap;
    std::unordered_map<std::string, int> tripMap;
    std::unordered_map<int, int> tripOriginalRoute;
    std::unordered_map<std::string, int> routeMap;

    std::vector<std::string> earliestArrival; // utils::LabelEarliestArrival
    std::vector<std::vector<utils::LabelEarliestArrival>> rounds;
    utils::IndexedVisitedSet stopsUpdated;
    utils::IndexedVisitedSet stopsReached;
    utils::IndexedVisitedHashMap routesServingUpdatedStops;

  };

} // strategy
// gtfs
// raptor

#endif //RAPTORSTRATEGY_H
