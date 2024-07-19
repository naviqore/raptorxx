//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef IRAPTORALGORITHMSTRATEGY_H
#define IRAPTORALGORITHMSTRATEGY_H

#include <types.h>


namespace raptor::utils {
  struct ConnectionRequest;
}
namespace raptor::strategy {

  class IRaptorAlgorithmStrategy
  {
  public:
    virtual ~IRaptorAlgorithmStrategy() = default;

    // virtual std::shared_ptr<IConnection> execute(utils::ConnectionRequest const& request) = 0;
    virtual  std::shared_ptr<IConnection> execute(const std::string& sourceStop, const std::string& targetStop, int departureTime) = 0;
  };
}

#endif //IRAPTORALGORITHMSTRATEGY_H
