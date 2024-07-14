//
// Created by MichaelBrunner on 06/07/2024.
//

#include "ConnectionImpl.h"

namespace raptor {
  ConnectionImpl::ConnectionImpl(std::vector<std::shared_ptr<Leg>>&& legs)
    : legs(std::move(legs)) {
  }
  std::vector<std::shared_ptr<Leg>> ConnectionImpl::getLegs() const {
    return legs;
  }
  std::tm ConnectionImpl::getDepartureTime() const {
    return {};
  }
  std::tm ConnectionImpl::getArrivalTime() const {
    return {};
  }
} // raptor