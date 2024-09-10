//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef ICONNECTION_H
#define ICONNECTION_H

#include "usingTypes.h"


#include <memory>
#include <vector>

namespace raptor {
  class Leg;
}

namespace raptor {
  class IConnection
  {
  public:
    virtual ~IConnection() = default;

    [[nodiscard]] virtual const std::vector<std::shared_ptr<Leg>>& getLegs() const = 0;

    [[nodiscard]] virtual types::raptorInt getDepartureTime() const = 0;

    [[nodiscard]] virtual types::raptorInt getArrivalTime() const = 0;
  };
}

#endif //ICONNECTION_H
