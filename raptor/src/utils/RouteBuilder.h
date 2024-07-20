//
// Created by MichaelBrunner on 20/07/2024.
//

// RouteBuilder.h
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include "RaptorData.h" // Assuming RaptorData.h contains the definitions for StopTime, Route, etc.
#include <raptor_export.h>


namespace raptor {

  class RouteContainer;

  class RAPTOR_API RouteBuilder
  {
  public:
    RouteBuilder(const std::string& routeId, const std::vector<std::string>& stopIds);

    void addTrip(const std::string& tripId);
    void addStopTime(const std::string& tripId, int position, const std::string& stopId, const StopTime& stopTime);
    RouteContainer build();

  private:
    void validate();

    std::string routeId_;
    std::map<int, std::string> stopSequence_;
    std::unordered_map<std::string, std::vector<StopTime>> trips_;
  };

  class RouteContainer
  {
  public:
    RouteContainer(const std::string& id, const std::map<int, std::string>& stopSequence, const std::map<std::string, std::vector<StopTime>>& trips);

    const std::string& id() const {
      return id_;
    }

    bool operator<(const RouteContainer& other) const;

    const std::map<int, std::string>& stopSequence() const {
      return stopSequence_;
    }

    const std::map<std::string, std::vector<StopTime>>& trips() const {
      return trips_;
    }

  private:
    std::string id_;
    std::map<int, std::string> stopSequence_;
    std::map<std::string, std::vector<StopTime>> trips_;
  };

} // namespace raptor
