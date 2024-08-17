//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorData.h"

#include <utility>

namespace raptor {

  RaptorData::RaptorData(StopRoutesIndexLookup  lookup, StopContext  stopContext, RouteTraversal  routeTraversal)
    : lookup(std::move(lookup))
    , stopContext(std::move(stopContext))
    , routeTraversal(std::move(routeTraversal)) {
  }

  const StopRoutesIndexLookup& RaptorData::getLookup() const {
    return this->lookup;
  }

  const StopContext& RaptorData::getStopContext() const {
    return this->stopContext;
  }

  const RouteTraversal& RaptorData::getRouteTraversal() const {
    return this->routeTraversal;
  }
} // raptor