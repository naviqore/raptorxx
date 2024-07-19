//
// Created by MichaelBrunner on 23/06/2024.
//

#include "Raptor.h"

#include "LoggerFactory.h"

#include <utility>
#include "raptorTypes.h"


namespace raptor {

  Raptor::Raptor(std::unique_ptr<strategy::IRaptorAlgorithmStrategy>&& strategy)
    : strategy(std::move(strategy)) {
    if (!this->strategy)
    {
      throw std::invalid_argument("strategy must not be null");
    }
  }

  std::shared_ptr<IConnection> Raptor::getConnections(utils::ConnectionRequest const& request) const {
    return strategy->execute(request.arrivalStopId.front(), request.departureStopId.front(), request.earliestDepartureTime);
  }
  void Raptor::testFunction() {
    getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info("Test function called");
  }


} // raptor