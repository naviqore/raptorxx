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
    const StopLabelsAndTimes::Label* previousLabel;

    ActiveTrip(const int tripOffset, const int entryTime, const StopLabelsAndTimes::Label* previousLabel)
      : tripOffset(tripOffset)
      , entryTime(entryTime)
      , previousLabel(previousLabel) {}
  };

}