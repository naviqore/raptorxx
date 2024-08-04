//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef RAPTOR_ROUTER_H
#define RAPTOR_ROUTER_H

#include "QueryConfig.h"
#include "RaptorAlgorithm.h"
#include "utils/RaptorData.h"

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <raptor_export.h>

namespace raptor {

  class RAPTOR_API RaptorRouter final : public RaptorAlgorithm
  {
  public:
    explicit RaptorRouter(RaptorData raptorData);

    [[nodiscard]] std::vector<std::unique_ptr<Connection>> routeEarliestArrival(const std::map<std::string, types::raptorInt>& departureStops, const std::map<std::string, types::raptorInt>& arrivalStops, const config::QueryConfig& config) const override;

    [[nodiscard]] std::vector<std::shared_ptr<Connection>> routeLatestDeparture(const std::map<std::string, types::raptorIdx>& departureStops, const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops, const config::QueryConfig& config) const override;

    [[nodiscard]] std::map<std::string, std::shared_ptr<Connection>> routeIsolines(const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops, const config::QueryConfig& config) const override;

    [[nodiscard]] const RaptorData& getRaptorData() const;


  private:
    [[nodiscard]] std::vector<std::shared_ptr<Connection>> getConnections(const std::map<std::string, types::raptorIdx>& sourceStops,
                                                                          const std::map<std::string, types::raptorIdx>& targetStops,
                                                                          const config::QueryConfig& config) const;

    RaptorData raptorData;
  };
}

#endif // RAPTOR_ROUTER_H
