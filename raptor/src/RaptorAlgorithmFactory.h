//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef RAPTORALGORITHMFACTORY_H
#define RAPTORALGORITHMFACTORY_H

#include "IRaptorAlgorithmFactory.h"
#include "strategies/RaptorStrategy.h"

#include <functional>
#include <map>
#include <raptor_export.h>

namespace raptor::strategy::factory {

  class RAPTOR_API RaptorAlgorithmFactory final : public IRaptorAlgorithmFactory
  {
  public:
    std::unique_ptr<IRaptorAlgorithmStrategy> create(AlgorithmType type, schedule::gtfs::TimetableManager&& relationManager) override;

  private:
    std::map<AlgorithmType, std::function<std::unique_ptr<IRaptorAlgorithmStrategy>(schedule::gtfs::TimetableManager&& relationManager)>> strategies{
      {RAPTOR, [](schedule::gtfs::TimetableManager&& relationManager) { return std::make_unique<RaptorStrategy>(std::move(relationManager)); }}};
  };

} // factory
// strategy
// gtfs
// raptor

#endif //RAPTORALGORITHMFACTORY_H
