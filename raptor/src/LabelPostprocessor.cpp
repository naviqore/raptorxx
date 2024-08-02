//
// Created by MichaelBrunner on 02/08/2024.
//

#include "LabelPostprocessor.h"

#include "LegImpl.h"
#include "RaptorConnection.h"
#include "RaptorRouter.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace raptor {

  bool canStopTimeBeSource(const StopTime& stopTime, types::raptorInt routeTargetTime, TimeType timeType) {
    return stopTime.arrival <= routeTargetTime;
  }

  bool canStopTimeBeTarget(const StopTime& stopTime, types::raptorInt routeSourceTime, TimeType timeType) {
    return stopTime.departure >= routeSourceTime;
  }


  LabelPostprocessor::LabelPostprocessor(const RaptorRouter& raptorData)
    : stops(raptorData.getRaptorData().getStopContext().stops)
    , stopTimes(raptorData.getRaptorData().getRouteTraversal().stopTimes)
    , routes(raptorData.getRaptorData().getRouteTraversal().routes)
    , routeStops(raptorData.getRaptorData().getRouteTraversal().routeStops) {
  }

  std::map<std::string, std::unique_ptr<Connection>> LabelPostprocessor::reconstructIsolines(const std::vector<std::vector<StopLabelsAndTimes::Label>>& bestLabelsPerRound, const types::raptorInt& referenceDate) const {
    std::map<std::string, std::unique_ptr<Connection>> isolines;
    for (auto i = 0; i < stops.size(); ++i)
    {
      Stop stop = stops[i];
      // if (auto bestLabelForStop = getBestLabelForStop(bestLabelsPerRound, i);
      //     bestLabelForStop && bestLabelForStop->type() != StopLabelsAndTimes::LabelType::INITIAL)
      // {
      //   if (auto connection = reconstructConnectionFromLabel(*bestLabelForStop, referenceDate))
      //   {
      //     isolines[stop.id] = std::move(std::make_unique<Connection>(connection.value()));
      //   }
      // }
    }
    return isolines;
  }

  std::vector<std::unique_ptr<Connection>> LabelPostprocessor::reconstructParetoOptimalSolutions(const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& bestLabelsPerRound,
                                                                                                 const std::map<types::raptorIdx, types::raptorIdx>& targetStops,
                                                                                                 const types::raptorInt& referenceDate) {
    std::vector<std::unique_ptr<Connection>> connections;
    for (const auto& labels : bestLabelsPerRound)
    {
      const StopLabelsAndTimes::Label* bestLabel = nullptr;
      auto bestTime = types::INFINITY_VALUE_MAX;

      for (const auto& [targetStopIdx, targetStopWalkingTime] : targetStops)
      {
        const auto currentLabel = labels[targetStopIdx].get();
        if (currentLabel == nullptr)
        {
          continue;
        }


        auto actualArrivalTime = currentLabel->targetTime + targetStopWalkingTime;
        if (actualArrivalTime < bestTime)
        {
          bestLabel = currentLabel;
          bestTime = actualArrivalTime;
        }
      }

      if (bestLabel == nullptr)
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

  std::unique_ptr<Connection> LabelPostprocessor::reconstructConnectionFromLabel(const StopLabelsAndTimes::Label* label, const types::raptorInt& referenceDate) {

    auto connection = std::make_unique<RaptorConnection>();
    std::vector<const StopLabelsAndTimes::Label*> labels;

    while (label->type != StopLabelsAndTimes::LabelType::INITIAL)
    {
      assert(label->previous);
      labels.push_back(label);
      label = label->previous;
    }

    maybeCombineFirstTwoLabels(labels);
    maybeCombineLastTwoLabels(labels);

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
        auto& [routeId, firstRouteStopIndex, numberOfStops, firstStopTimeIndex, numberOfTrips, tripIds] = routes[currentLabel->routeOrTransferIdx];
        tripId = tripIds[currentLabel->tripOffset];
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

      // Convert timestamps to LocalDateTime if necessary
      // auto departureTime = DateTimeUtils::convertToLocalDateTime(departureTimestamp, referenceDate);
      // auto arrivalTime = DateTimeUtils::convertToLocalDateTime(arrivalTimestamp, referenceDate);

      connection->addLeg(std::make_shared<LegImpl>(routeId, tripId, fromStopId, toStopId, departureTimestamp, arrivalTimestamp, type));
    }

    if (!connection->getLegs().empty())
    {
      connection->initialize();
      return connection;
    }
    return nullptr;
  }

  void LabelPostprocessor::maybeCombineFirstTwoLabels(std::vector<const StopLabelsAndTimes::Label*>& labels) {
    maybeCombineLabels(labels, true);
  }

  void LabelPostprocessor::maybeCombineLastTwoLabels(std::vector<const StopLabelsAndTimes::Label*>& labels) {
    maybeCombineLabels(labels, false);
  }

  void LabelPostprocessor::maybeCombineLabels(std::vector<const StopLabelsAndTimes::Label*>& labels, const bool fromTarget) {
    if (labels.size() < 2)
    {
      return;
    }

    auto transferLabelIndex = fromTarget ? 0 : labels.size() - 1;
    const auto routeLabelIndex = fromTarget ? 1 : labels.size() - 2;

    auto& transferLabel = labels[transferLabelIndex];
    auto& routeLabel = labels[routeLabelIndex];

    if (transferLabel->type != StopLabelsAndTimes::LabelType::TRANSFER || routeLabel->type != StopLabelsAndTimes::LabelType::ROUTE)
    {
      return;
    }

    const int stopIdx = fromTarget ? transferLabel->stopIdx : transferLabel->previous->stopIdx;

    auto stopTime = getTripStopTimeForStopInTrip(stopIdx, routeLabel->routeOrTransferIdx, routeLabel->tripOffset);

    // if (!stopTime || (fromTarget ? !canStopTimeBeTarget(*stopTime, routeLabel->targetTime(), TimeType::DEPARTURE) : !canStopTimeBeSource(*stopTime, routeLabel->sourceTime(), TimeType::ARRIVAL)))
    // {
    //   if (!fromTarget)
    //   {
    //     maybeShiftSourceTransferCloserToFirstRoute(labels, transferLabel, routeLabel, transferLabelIndex);
    //   }
    //   return;
    // }
    //
    // bool isDeparture = timeType == TimeType::DEPARTURE;
    //
    // if (isDeparture)
    // {
    //   transferLabel->setTargetTime(stopTime->arrivalTime());
    // }
    // else
    // {
    //   transferLabel->setSourceTime(stopTime->departureTime());
    // }
    //
    // labels.erase(labels.begin() + routeLabelIndex);
  }

  void LabelPostprocessor::maybeShiftSourceTransferCloserToFirstRoute(std::vector<const StopLabelsAndTimes::Label*>& labels,
                                                                      const StopLabelsAndTimes::Label& transferLabel,
                                                                      const StopLabelsAndTimes::Label& routeLabel,
                                                                      int transferLabelIndex) {
    if (labels.size() < 3)
    {
      return;
    }

    auto& nextLabel = labels[transferLabelIndex + 1];
    if (nextLabel->type != StopLabelsAndTimes::LabelType::TRANSFER)
    {
      return;
    }

    auto transferStartTime = transferLabel.sourceTime;
    auto routeStartTime = routeLabel.sourceTime;
    auto timeDifference = transferStartTime - routeStartTime;

    if (timeDifference > 0 && timeDifference < nextLabel->sourceTime - transferStartTime)
    {
      labels.erase(labels.begin() + transferLabelIndex);
    }
  }

  std::optional<StopTime> LabelPostprocessor::getTripStopTimeForStopInTrip(int stopIdx, int routeIdx, int tripOffset) {

    return std::nullopt;
  }


} // raptor