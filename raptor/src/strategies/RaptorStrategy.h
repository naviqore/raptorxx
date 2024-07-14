//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef RAPTORSTRATEGY_H
#define RAPTORSTRATEGY_H

#include "IConnection.h"
#include "IRaptorAlgorithmStrategy.h"
#include "Raptor.h"

namespace raptor::strategy {

  class RaptorStrategy final : public IRaptorAlgorithmStrategy
  {
    schedule::gtfs::RelationManager relationManager;

  public:
    explicit RaptorStrategy(schedule::gtfs::RelationManager&& relationManager);

    std::shared_ptr<IConnection> execute(const utils::ConnectionRequest& request) override;
  };

} // strategy
// gtfs
// raptor

#endif //RAPTORSTRATEGY_H
