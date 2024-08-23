//
// Created by MichaelBrunner on 26/07/2024.
//

#include "config/QueryConfig.h"


#include <stdexcept>

namespace raptor::config {

  QueryConfig::QueryConfig()
    : maximumWalkingDuration(types::INFINITY_VALUE_MAX)
    , minimumTransferDuration(0)
    , maximumTransferNumber(types::INFINITY_VALUE_MAX)
    , maximumTravelTime(types::INFINITY_VALUE_MAX) {}


  QueryConfig::QueryConfig(const WalkingConfig& walkingConfig, const TransferConfig& transferConfig, const TravelConfig& travelConfig)
    : maximumWalkingDuration(walkingConfig.getMaxWalkingDuration())
    , minimumTransferDuration(transferConfig.getMinTransferDuration())
    , maximumTransferNumber(transferConfig.getMaxTransferNumber())
    , maximumTravelTime(travelConfig.getMaxTravelTime()) {
  }

  auto QueryConfig::getMaximumWalkingDuration() const -> types::raptorInt {
    return maximumWalkingDuration;
  }

  auto QueryConfig::getMinimumTransferDuration() const -> types::raptorInt {
    return minimumTransferDuration;
  }

  auto QueryConfig::getMaximumTransferNumber() const -> types::raptorInt {
    return maximumTransferNumber;
  }

  auto QueryConfig::getMaximumTravelTime() const -> types::raptorInt {
    return maximumTravelTime;
  }

  void QueryConfig::setMaximumWalkingDuration(const types::raptorInt maxWalkingDuration) {
    maximumWalkingDuration = maxWalkingDuration;
  }

  void QueryConfig::setMinimumTransferDuration(const types::raptorInt minTransferDuration) {
    minimumTransferDuration = minTransferDuration;
  }

  void QueryConfig::setMaximumTransferNumber(const types::raptorInt maxTransferNumber) {
    maximumTransferNumber = maxTransferNumber;
  }

  void QueryConfig::setMaximumTravelTime(const types::raptorInt maxTravelTime) {
    maximumTravelTime = maxTravelTime;
  }
}
