//
// Created by MichaelBrunner on 06/07/2024.
//

#ifndef CONNECTION_H
#define CONNECTION_H

#include "IConnection.h"

namespace raptor {

  class ConnectionImpl : public IConnection
  {
    std::vector<std::shared_ptr<Leg>> legs;

  public:
    explicit ConnectionImpl(std::vector<std::shared_ptr<Leg>>&& legs);

    [[nodiscard]] std::vector<std::shared_ptr<Leg>> getLegs() const override;

    [[nodiscard]] std::tm getDepartureTime() const override;

    [[nodiscard]] std::tm getArrivalTime() const override;
  };

} // raptor

#endif //CONNECTION_H
