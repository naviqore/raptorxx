//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef RAPTOR_H
#define RAPTOR_H

#include "IRaptor.h"


#include <IConnection.h>
#include <gtfs/RelationManager.h>
#include <IRaptorAlgorithmStrategy.h>
#include <memory>
#include <raptor_export.h>

namespace raptor {
  namespace utils {
    struct ConnectionRequest;
  }

  class RAPTOR_API Raptor final : public IRaptor
  {
    std::unique_ptr<strategy::IRaptorAlgorithmStrategy> strategy;

  public:
    explicit Raptor(std::unique_ptr<strategy::IRaptorAlgorithmStrategy>&& strategy);

    [[nodiscard]] std::shared_ptr<IConnection> getConnections(utils::ConnectionRequest const& request) const override;

    void testFunction() override;
  };

} // raptor

#endif //RAPTOR_H
