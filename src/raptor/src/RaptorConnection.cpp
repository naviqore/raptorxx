//
// Created by MichaelBrunner on 02/08/2024.
//

#include "RaptorConnection.h"
#include <algorithm>
#include <numeric>

namespace raptor {

  void RaptorConnection::validateLegOrder(const std::shared_ptr<Leg>& current, const std::shared_ptr<Leg>& next)
  {
    if (current->getToStopId() != next->getFromStopId()) {
      throw std::invalid_argument(std::format("Legs are not connected: {} -> {}", current->getToStopId(), next->getFromStopId()));
    }
    if (current->getArrivalTime() < current->getDepartureTime()) {
      throw std::invalid_argument("Arrival time must be after departure time.");
    }
    if (current->getArrivalTime() > next->getDepartureTime()) {
      throw std::invalid_argument("Arrival time must be before next departure time.");
    }
  }

  void RaptorConnection::addLeg(const std::shared_ptr<Leg>& leg)
  {
    legs.push_back(leg);
  }

  void RaptorConnection::initialize()
  {

    std::ranges::sort(legs, [](const auto& aLhs, const auto& aRhs) {
      return *aLhs < *aRhs;
    });
    for (auto i = 0; i < legs.size() - 1; ++i) {
      validateLegOrder(legs[i], legs[i + 1]);
    }
    legs.shrink_to_fit();
  }

  int RaptorConnection::getDepartureTime() const
  {
    return legs.front()->getDepartureTime();
  }

  int RaptorConnection::getArrivalTime() const
  {
    return legs.back()->getArrivalTime();
  }

  std::string RaptorConnection::getFromStopId() const
  {
    return legs.front()->getFromStopId();
  }

  std::string RaptorConnection::getToStopId() const
  {
    return legs.back()->getToStopId();
  }

  int RaptorConnection::getDurationInSeconds() const
  {
    return getArrivalTime() - getDepartureTime();
  }

  std::vector<std::shared_ptr<Leg>> RaptorConnection::getWalkTransfers() const
  {
    std::vector<std::shared_ptr<Leg>> walkTransfers;
    walkTransfers.reserve(legs.size());
    std::ranges::copy_if(legs, std::back_inserter(walkTransfers), [](const auto& leg) { return leg->getType() == Leg::Type::WALK_TRANSFER; });
    walkTransfers.shrink_to_fit();
    return walkTransfers;
  }

  std::vector<std::shared_ptr<Leg>> RaptorConnection::getRouteLegs() const
  {
    std::vector<std::shared_ptr<Leg>> routeLegs;
    routeLegs.reserve(legs.size());
    std::ranges::copy_if(legs, std::back_inserter(routeLegs), [](const auto& leg) { return leg->getType() == Leg::Type::ROUTE; });
    routeLegs.shrink_to_fit();
    return routeLegs;
  }

  types::raptorInt RaptorConnection::getNumberOfSameStopTransfers() const
  {
    types::raptorInt transferCounter = 0;
    for (size_t i = 0; i < legs.size() - 1; ++i) {
      if (legs[i]->getType() == Leg::Type::ROUTE
          && legs[i + 1]->getType() == Leg::Type::ROUTE) {
        ++transferCounter;
      }
    }
    return transferCounter;
  }

  types::raptorInt RaptorConnection::getNumberOfTotalTransfers() const
  {
    return static_cast<types::raptorInt>(getWalkTransfers().size() + getNumberOfSameStopTransfers());
  }

  const std::vector<std::shared_ptr<Leg>>& RaptorConnection::getLegs() const
  {
    return legs;
  }
} // raptor
