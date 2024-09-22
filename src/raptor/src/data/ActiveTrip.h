//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include "../utils/StopLabelsAndTimes.h"

namespace raptor {

  struct ActiveTrip
  {
    types::raptorInt tripOffset;
    types::raptorInt entryTime;
    const StopLabelsAndTimes::Label* previousLabel;

    ActiveTrip(const types::raptorInt tripOffset, const types::raptorInt entryTime, const StopLabelsAndTimes::Label* previousLabel)
      : tripOffset(tripOffset)
      , entryTime(entryTime)
      , previousLabel(previousLabel) {}
  };

}