//
// Created by MichaelBrunner on 27/06/2024.
//

#include "RaptorStrategy.h"
#include <LoggingPool.h>
#include <format>
#include "raptorTypes.h"

namespace raptor::strategy {

  std::shared_ptr<IConnection> RaptorStrategy::execute(const utils::ConnectionRequest& request) {

    LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Agency: {} {} {}", request.departureStop.stopName, request.departureTime, request.arrivalStop.stopName));

    return nullptr;
  }
} // strategy
  // gtfs
  // raptor