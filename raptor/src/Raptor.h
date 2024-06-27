//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef RAPTOR_H
#define RAPTOR_H
#include "IConnection.h"


#include <utility>

#include <gtfs/GtfsData.h>
#include <gtfs/RelationManager.h>
#include <IRaptorAlgorithmStrategy.h>
#include <memory>
#include <raptor_export.h>

namespace raptor {
  namespace utils {
    struct ConnectionRequest;
  }

  class RAPTOR_API Raptor
  {
    std::unique_ptr<strategy::IRaptorAlgorithmStrategy> strategy;
    schedule::gtfs::RelationManager relationManager;

  public:
    explicit Raptor(std::unique_ptr<strategy::IRaptorAlgorithmStrategy>&& strategy, schedule::gtfs::RelationManager&& relationManager);

    void getConnections(utils::ConnectionRequest const& request) const;
  };

} // raptor

#endif //RAPTOR_H
