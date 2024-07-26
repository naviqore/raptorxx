//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include "StopLabelsAndTimes.h"
#include <memory>

namespace raptor {

  struct ActiveTrip
  {
    int tripOffset;
    int entryTime;
    std::shared_ptr<StopLabelsAndTimes::Label> previousLabel;

    ActiveTrip(const int tripOffset, const int entryTime, std::shared_ptr<StopLabelsAndTimes::Label>&& previousLabel)
      : tripOffset(tripOffset)
      , entryTime(entryTime)
      , previousLabel(std::move(previousLabel)) {}
  };

}