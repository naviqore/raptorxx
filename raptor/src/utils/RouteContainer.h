//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include "raptorRouteStructures.h"


#include <string>
#include <map>
#include <vector>


namespace raptor {

  class RouteContainer
  {
  public:
    RouteContainer(std::string  id, const std::map<int, std::string>& stopSequence, const std::map<std::string, std::vector<StopTime>>& trips);

    [[nodiscard]] const std::string& id() const;

    bool operator<(const RouteContainer& other) const;

    [[nodiscard]] const std::map<int, std::string>& stopSequence() const;

    [[nodiscard]] const std::map<std::string, std::vector<StopTime>>& trips() const;

  private:
    std::string routeContainerId;
    std::map<int, std::string> StopSequenceMap;
    std::map<std::string, std::vector<StopTime>> idTripsMap;
  };

} // raptor
