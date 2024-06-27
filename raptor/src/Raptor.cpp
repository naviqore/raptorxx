//
// Created by MichaelBrunner on 23/06/2024.
//

#include "Raptor.h"

#include <utility>
#include "raptorTypes.h"


namespace raptor {

  Raptor::Raptor(std::unique_ptr<strategy::IRaptorAlgorithmStrategy>&& strategy, schedule::gtfs::RelationManager&& relationManager)
    : strategy(std::move(strategy))
    , relationManager(std::move(relationManager)) {
    if (nullptr == strategy)
    {
      throw std::invalid_argument("strategy must not be null");
    }
  }

  void Raptor::getConnections(utils::ConnectionRequest const& request) const {
    strategy->execute(request);
  }


} // raptor