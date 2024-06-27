//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef IRAPTORALGORITHMFACTORY_H
#define IRAPTORALGORITHMFACTORY_H

#include <memory>
#include <raptor_export.h>

namespace raptor::strategy {
  class IRaptorAlgorithmStrategy;
}

namespace raptor::strategy::factory {

  class RAPTOR_API IRaptorAlgorithmFactory
  {
  public:
    enum AlgorithmType
    {
      RAPTOR
    };

    virtual ~IRaptorAlgorithmFactory() = default;

    virtual std::unique_ptr<IRaptorAlgorithmStrategy> create(AlgorithmType type) = 0;
  };
}

#endif //IRAPTORALGORITHMFACTORY_H
