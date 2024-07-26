//
// Created by MichaelBrunner on 20/07/2024.
//

#pragma once
#include "TravelConfig.h"
#include "usingTypes.h"


#include <TransferConfig.h>
#include <WalkingConfig.h>
#include <raptor_export.h>

namespace raptor::config {
  class RAPTOR_API QueryConfig
  {
  public:
    QueryConfig();

    // QueryConfig(int maxWalkingDuration, int minTransferDuration, int maxTransferNumber, int maxTravelTime);

    explicit QueryConfig(const WalkingConfig& walkingConfig, const TransferConfig& transferConfig, const TravelConfig& travelConfig);

    [[nodiscard]] auto getMaximumWalkingDuration() const -> types::raptorInt;
    [[nodiscard]] auto getMinimumTransferDuration() const -> types::raptorInt;
    [[nodiscard]] auto getMaximumTransferNumber() const -> types::raptorInt;
    [[nodiscard]] auto getMaximumTravelTime() const -> types::raptorInt;

    void setMaximumWalkingDuration(types::raptorInt maxWalkingDuration);
    void setMinimumTransferDuration(types::raptorInt minTransferDuration);
    void setMaximumTransferNumber(types::raptorInt maxTransferNumber);
    void setMaximumTravelTime(types::raptorInt maxTravelTime);

  private:
    types::raptorInt maximumWalkingDuration;
    types::raptorInt minimumTransferDuration;
    types::raptorInt maximumTransferNumber;
    types::raptorInt maximumTravelTime;
  };

}