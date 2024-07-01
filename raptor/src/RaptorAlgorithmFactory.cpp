//
// Created by MichaelBrunner on 27/06/2024.
//

#include "RaptorAlgorithmFactory.h"


namespace raptor::strategy::factory {

  std::unique_ptr<IRaptorAlgorithmStrategy> RaptorAlgorithmFactory::create(const AlgorithmType type, schedule::gtfs::RelationManager&& relationManager) {
    return strategies[type](std::move(relationManager));
  }
} // factory
  // strategy
  // gtfs
  // raptor