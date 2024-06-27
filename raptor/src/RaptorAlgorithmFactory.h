//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef RAPTORALGORITHMFACTORY_H
#define RAPTORALGORITHMFACTORY_H

#include "IRaptorAlgorithmFactory.h"
#include "RaptorStrategy.h"

#include <functional>
#include <map>
#include <raptor_export.h>

namespace raptor::strategy::factory {

  class RAPTOR_API RaptorAlgorithmFactory final : public IRaptorAlgorithmFactory
  {
  public:
    std::unique_ptr<IRaptorAlgorithmStrategy> create(AlgorithmType type) override;

  private:
    std::map<AlgorithmType, std::function<std::unique_ptr<IRaptorAlgorithmStrategy>()>> strategies{
      {AlgorithmType::RAPTOR, []() { return std::make_unique<RaptorStrategy>(); }}};
  };

} // factory
// strategy
// gtfs
// raptor

#endif //RAPTORALGORITHMFACTORY_H
