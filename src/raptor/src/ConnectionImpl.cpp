//
// Created by MichaelBrunner on 06/07/2024.
//

#include "ConnectionImpl.h"

namespace raptor {
  ConnectionImpl::ConnectionImpl(std::vector<std::shared_ptr<Leg>>&& legs)
    : legs(std::move(legs))
  {
  }
  const std::vector<std::shared_ptr<Leg>>& ConnectionImpl::getLegs() const {
    return legs;
  }
 types::raptorInt ConnectionImpl::getDepartureTime() const {
    return {};
  }
types::raptorInt ConnectionImpl::getArrivalTime() const {
    return {};
  }
} // raptor