//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef ICONNECTION_H
#define ICONNECTION_H

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

    [[nodiscard]] virtual std::vector<std::shared_ptr<Leg>> getLegs() const = 0;

    [[nodiscard]] virtual std::tm getDepartureTime() const = 0;

    [[nodiscard]] virtual std::tm getArrivalTime() const = 0;
  };
}

#endif //ICONNECTION_H
