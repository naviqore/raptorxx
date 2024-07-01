//
// Created by MichaelBrunner on 01/07/2024.
//

#ifndef IRAPTOR_H
#define IRAPTOR_H

#include "IConnection.h"
#include <raptor_export.h>

namespace raptor {
  namespace utils {
    struct ConnectionRequest;
  }

  class RAPTOR_API IRaptor
  {
  public:
    virtual ~IRaptor() = default;

    [[nodiscard]] virtual std::shared_ptr<IConnection> getConnections(utils::ConnectionRequest const& request) const = 0;
  };
}

#endif //IRAPTOR_H
