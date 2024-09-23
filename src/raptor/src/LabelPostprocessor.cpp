//
// Created by MichaelBrunner on 02/08/2024.
//

#include "LabelPostprocessor.h"

#include "LegImpl.h"
#include "RaptorConnection.h"
#include "include/RaptorRouter.h"
#include "RouteScanner.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <format>

#include "utils/scopedTimer.h"

namespace raptor {


  LabelPostprocessor::LabelPostprocessor(const RaptorRouter& raptorData)
    : stops(raptorData.getRaptorData().getStopContext().stops)
    , stopTimes(raptorData.getRaptorData().getRouteTraversal().stopTimes)
    , routes(raptorData.getRaptorData().getRouteTraversal().routes)
    , routeStops(raptorData.getRaptorData().getRouteTraversal().routeStops)
  {
  }


  std::vector<std::unique_ptr<Connection>> LabelPostprocessor::reconstructParetoOptimalSolutions(const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& bestLabelsPerRound,
                                                                                                 const std::map<types::raptorIdx, types::raptorIdx>& targetStops,
                                                                                                 const types::raptorInt& referenceDate) const
  {
    MEASURE_FUNCTION();
    std::vector<std::unique_ptr<Connection>> connections{};

    for (const auto& labels : bestLabelsPerRound) {
      const StopLabelsAndTimes::Label* bestLabel = nullptr;
      auto bestTime = types::INFINITY_VALUE_MAX;

      for (const auto& [targetStopIdx, targetStopWalkingTime] : targetStops) {
        const auto currentLabel = labels[targetStopIdx].get();
        if (nullptr == currentLabel) {
          continue;
        }

        const auto actualArrivalTime = currentLabel->targetTime + targetStopWalkingTime;
        if (actualArrivalTime < bestTime) {
          bestLabel = currentLabel;
          bestTime = actualArrivalTime;
        }
      }

      if (nullptr == bestLabel) {
        continue;
      }

      if (auto connection = reconstructConnectionFromLabel(bestLabel)) {
        connections.push_back(std::move(connection));
      }
    }
    return connections;
  }

  std::unique_ptr<Connection> LabelPostprocessor::reconstructConnectionFromLabel(const StopLabelsAndTimes::Label* label) const
  {
    MEASURE_FUNCTION();

    std::vector<const StopLabelsAndTimes::Label*> labels;
    // labels.reserve(12); // Wild guess for the number of labels - can improve about 1ms

    while (label->type != StopLabelsAndTimes::LabelType::INITIAL) {
      labels.push_back(label);
      label = label->previous;
    }
    labels.shrink_to_fit();
    auto connection = std::make_unique<RaptorConnection>();
    for (const auto currentLabel : labels) {
      std::string routeId;
      std::string tripId;
      std::string fromStopId;
      std::string toStopId;
      Leg::Type type;

      fromStopId = stops[currentLabel->previous->stopIdx].id;
      toStopId = stops[currentLabel->stopIdx].id;

      const types::raptorIdx departureTimestamp = currentLabel->sourceTime;
      const types::raptorIdx arrivalTimestamp = currentLabel->targetTime;

      if (currentLabel->type == StopLabelsAndTimes::LabelType::ROUTE) {
        const auto& route = routes[currentLabel->routeOrTransferIdx];
        tripId = route.tripIds[currentLabel->tripOffset];
        routeId = route.id;
        type = Leg::Type::ROUTE;
      }
      else if (currentLabel->type == StopLabelsAndTimes::LabelType::TRANSFER) {
        routeId = std::string("transfer_").append(fromStopId).append("_").append(toStopId);
        type = Leg::Type::WALK_TRANSFER;
      }
      else {
        throw std::runtime_error("Unknown label type");
      }

      connection->addLeg(std::make_shared<LegImpl>(std::move(routeId),
                                                   std::move(tripId),
                                                   std::move(fromStopId),
                                                   std::move(toStopId),
                                                   departureTimestamp,
                                                   arrivalTimestamp,
                                                   type));
    }

    if (!connection->getLegs().empty()) {
      connection->initialize();
      return connection;
    }
    return nullptr;
  }


} // raptor
