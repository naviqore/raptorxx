//
// Created by MichaelBrunner on 20/07/2024.
//

// RouteBuilder.h
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include "RouteContainer.h"

#include <raptor_export.h>


namespace raptor {

  class RAPTOR_API RouteBuilder
  {
  public:
    RouteBuilder(const std::string& routeId, const std::vector<std::string>& stopIds);

    void addTrip(const std::string& tripId);

    void addStopTime(const std::string& tripId, int position, const std::string& stopId, const StopTime& stopTime);

    RouteContainer build();

  private:
    std::string routeId;

    std::map<int, std::string> stopSequence;

    std::unordered_map<std::string, std::vector<StopTime>> trips;
  };

} // namespace raptor
