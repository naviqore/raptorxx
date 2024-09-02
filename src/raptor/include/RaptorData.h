//
// Created by MichaelBrunner on 20/07/2024.
//

#pragma once

#include <raptorRouteStructures.h>
#include <raptor_export.h>

namespace raptor {


  class RAPTOR_API RaptorData {
    StopRoutesIndexLookup lookup;
    StopContext stopContext;
    RouteTraversal routeTraversal;

  public:
    explicit RaptorData(StopRoutesIndexLookup lookup, StopContext stopContext, RouteTraversal routeTraversal);

    [[nodiscard]] const StopRoutesIndexLookup& getLookup() const;

    [[nodiscard]] const StopContext& getStopContext() const;

    [[nodiscard]] const RouteTraversal& getRouteTraversal() const;
  };

} // raptor
