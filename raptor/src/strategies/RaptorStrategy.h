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

    schedule::gtfs::TimetableManager timeTable;
    static constexpr size_t MAX_ROUNDS = 5;
    std::string sourceStop;
    std::string targetStop;
    int startTime{};
    std::unordered_map<std::string, std::string> routesToScan;    // routeId -> stopId
    std::unordered_map<std::string, std::vector<schedule::gtfs::SubRoute>> subRoutesToScan; // subRouteId -> stopId

  public:
    explicit RaptorStrategy(schedule::gtfs::TimetableManager&& relationManager);

    RaptorStrategy(const RaptorStrategy&) = delete;
    RaptorStrategy& operator=(const RaptorStrategy&) = delete;
    RaptorStrategy(RaptorStrategy&&) noexcept = default;
    RaptorStrategy& operator=(RaptorStrategy&&) = default;

    std::shared_ptr<IConnection> execute(const std::string& sourceStop, const std::string& targetStop, unsigned int departureTime) override;

  private:
    void initialize();
    void updateRoutesToScan();
    void scanRoutes(size_t currentRoundK);
    void scanTransfers(size_t currentRoundK);
  };

} // strategy
// gtfs
// raptor

#endif //RAPTORSTRATEGY_H
