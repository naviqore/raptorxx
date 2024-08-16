//
// Created by MichaelBrunner on 02/08/2024.
//

#include "LabelPostprocessor.h"

#include "LegImpl.h"
#include "RaptorConnection.h"
#include "RaptorRouter.h"
#include "RouteScanner.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace raptor {


  LabelPostprocessor::LabelPostprocessor(const RaptorRouter& raptorData)
    : stops(raptorData.getRaptorData().getStopContext().stops)
    , stopTimes(raptorData.getRaptorData().getRouteTraversal().stopTimes)
    , routes(raptorData.getRaptorData().getRouteTraversal().routes)
    , routeStops(raptorData.getRaptorData().getRouteTraversal().routeStops) {
  }


  std::vector<std::unique_ptr<Connection>> LabelPostprocessor::reconstructParetoOptimalSolutions(const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& bestLabelsPerRound,
                                                                                                 const std::map<types::raptorIdx, types::raptorIdx>& targetStops,
                                                                                                 const types::raptorInt& referenceDate) const {
    std::vector<std::unique_ptr<Connection>> connections{};
    for (const auto& labels : bestLabelsPerRound)
    {
      const StopLabelsAndTimes::Label* bestLabel = nullptr;
      auto bestTime = types::INFINITY_VALUE_MAX;

      for (const auto& [targetStopIdx, targetStopWalkingTime] : targetStops)
      {
        const auto currentLabel = labels[targetStopIdx].get();
        if (nullptr == currentLabel)
        {
          continue;
        }

        const auto actualArrivalTime = currentLabel->targetTime + targetStopWalkingTime;
        if (actualArrivalTime < bestTime)
        {
          bestLabel = currentLabel;
          bestTime = actualArrivalTime;
        }
      }

      if (nullptr == bestLabel)
      {
        continue;
      }

      if (auto connection = reconstructConnectionFromLabel(bestLabel, referenceDate))
      {
        connections.push_back(std::move(connection));
      }
    }
    return connections;
  }

  std::unique_ptr<Connection> LabelPostprocessor::reconstructConnectionFromLabel(const StopLabelsAndTimes::Label* label, const types::raptorInt& referenceDate) const {

    auto connection = std::make_unique<RaptorConnection>();
    std::vector<const StopLabelsAndTimes::Label*> labels;

    while (label->type != StopLabelsAndTimes::LabelType::INITIAL)
    {
      assert(label->previous);
      labels.push_back(label);
      label = label->previous;
    }

    for (const auto& currentLabel : labels)
    {
      std::string routeId;
      std::string tripId;
      std::string fromStopId;
      std::string toStopId;
      Leg::Type type;

      assert(currentLabel->previous != nullptr);

      fromStopId = stops[currentLabel->previous->stopIdx].id;
      toStopId = stops[currentLabel->stopIdx].id;


      const types::raptorIdx departureTimestamp = currentLabel->sourceTime;
      const types::raptorIdx arrivalTimestamp = currentLabel->targetTime;

      if (currentLabel->type == StopLabelsAndTimes::LabelType::ROUTE)
      {
        const auto& route = routes[currentLabel->routeOrTransferIdx];
        tripId = route.tripIds[currentLabel->tripOffset];
        routeId = route.id;
        type = Leg::Type::ROUTE;
      }
      else if (currentLabel->type == StopLabelsAndTimes::LabelType::TRANSFER)
      {
        routeId = std::format("transfer_{}_{}", fromStopId, toStopId);
        type = Leg::Type::WALK_TRANSFER;
      }
      else
      {
        throw std::runtime_error("Unknown label type");
      }

      connection->addLeg(std::make_shared<LegImpl>(routeId, tripId, fromStopId, toStopId, departureTimestamp, arrivalTimestamp, type));
    }

    if (!connection->getLegs().empty())
    {
      connection->initialize();
      return connection;
    }
    return nullptr;
  }


} // raptor