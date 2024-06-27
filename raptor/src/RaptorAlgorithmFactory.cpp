//
// Created by MichaelBrunner on 27/06/2024.
//

#include "RaptorAlgorithmFactory.h"


namespace raptor::strategy::factory {

  std::unique_ptr<IRaptorAlgorithmStrategy> RaptorAlgorithmFactory::create(AlgorithmType type) {
    return strategies[type]();
  }
} // factory
  // strategy
  // gtfs
  // raptor