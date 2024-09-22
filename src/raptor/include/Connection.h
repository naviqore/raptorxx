//
// Created by MichaelBrunner on 02/08/2024.
//

#pragma once

#include "usingTypes.h"


#include <chrono>

namespace raptor {
  class Leg;

  class Connection
  {
  public:
    virtual ~Connection() = default;

    [[nodiscard]] virtual int getDepartureTime() const = 0;

    [[nodiscard]] virtual int getArrivalTime() const = 0;

    [[nodiscard]] virtual std::string getFromStopId() const = 0;

    [[nodiscard]] virtual std::string getToStopId() const = 0;

    [[nodiscard]] virtual int getDurationInSeconds() const = 0;

    [[nodiscard]] virtual std::vector<std::shared_ptr<Leg>> getWalkTransfers() const = 0;

    [[nodiscard]] virtual std::vector<std::shared_ptr<Leg>> getRouteLegs() const = 0;

    [[nodiscard]] virtual types::raptorInt getNumberOfSameStopTransfers() const = 0;

    [[nodiscard]] virtual types::raptorInt getNumberOfTotalTransfers() const = 0;

    [[nodiscard]] virtual const std::vector<std::shared_ptr<Leg>>& getLegs() const = 0;
  };
}
