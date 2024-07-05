//
// Created by MichaelBrunner on 13/06/2024.
//

#include "RelationManager.h"

#include <algorithm>
#include <ranges>


namespace schedule::gtfs {

  RelationManager::RelationManager(const GtfsData& data)
    : data(data) {
    createRelations();
  }

  void RelationManager::createRelations() {
  }

  const GtfsData& RelationManager::getData() const {
    return data;
  }

  const std::string& RelationManager::getStopNameFromStopId(std::string const& aStopId) const {
    return data.stops.at(aStopId).stopName;
  }
  const std::string& RelationManager::getStopIdFromStopName(std::string const& aStopName) const {
    return std::ranges::find_if(data.stops, [&aStopName](const auto& stop) {
             return stop.second.stopName == aStopName;
           })
      ->first;
  }



} // gtfs