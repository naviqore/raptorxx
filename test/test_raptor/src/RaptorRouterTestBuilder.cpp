//
// Created by MichaelBrunner on 26/07/2024.
//


#include "RaptorRouterTestBuilder.h"

#include <algorithm>
#include <ranges>


Route::Route(std::string id, std::vector<std::string> stops, const int firstDepartureOffset, const int headWayTime, const int travelTimeBetweenStops, const int dwellTimeAtStop)
  : id(std::move(id))
  , stops(std::move(stops))
  , firstDepartureOffset(firstDepartureOffset)
  , headWayTime(headWayTime)
  , travelTimeBetweenStops(travelTimeBetweenStops)
  , dwellTimeAtStop(dwellTimeAtStop) {}

Route::Route(std::string id, std::vector<std::string> stops)
  : Route(std::move(id), std::move(stops), DEFAULT_OFFSET, DEFAULT_HEADWAY_TIME, DEFAULT_TIME_BETWEEN_STOPS, DEFAULT_DWELL_TIME) {}

Transfer::Transfer(std::string sourceStop, std::string targetStop, const int duration)
  : sourceStop(std::move(sourceStop))
  , targetStop(std::move(targetStop))
  , duration(duration) {}

RaptorRouterTestBuilder::RaptorRouterTestBuilder()
  : sameStopTransferTime(120) {}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute1_AG() {
  return withAddRoute1_AG(DEFAULT_OFFSET, DEFAULT_HEADWAY_TIME, DEFAULT_TIME_BETWEEN_STOPS, DEFAULT_DWELL_TIME);
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute1_AG(const int offset, const int headway, const int travelTime, const int dwellTime) {
  return withAddRoute1_AG("R1", offset, headway, travelTime, dwellTime);
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute1_AG(const std::string& routeId, int offset, int headway, int travelTime, int dwellTime) {
  routes.emplace_back(routeId, std::vector<std::string>{"A", "B", "C", "D", "E", "F", "G"}, offset, headway, travelTime, dwellTime);
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute2_HL() {
  routes.emplace_back("R2", std::vector<std::string>{"H", "B", "I", "J", "K", "L"});
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute3_MQ() {
  routes.emplace_back("R3", std::vector<std::string>{"M", "K", "N", "O", "P", "Q"});
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute3_MQ(int offset, int headway, int travelTime, int dwellTime) {
  routes.emplace_back("R3", std::vector<std::string>{"M", "K", "N", "O", "P", "Q"}, offset, headway, travelTime, dwellTime);
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute4_RS() {
  routes.emplace_back("R4", std::vector<std::string>{"R", "P", "F", "S"});
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddRoute5_AH_selfIntersecting() {
  routes.emplace_back("R5", std::vector<std::string>{"A", "B", "C", "D", "E", "F", "P", "O", "N", "K", "J", "I", "B", "H"});
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddTransfer1_ND() {
  return withAddTransfer1_ND(60);
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddTransfer1_ND(int duration) {
  transfers.emplace_back("N", "D", duration);
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddTransfer2_LR() {
  transfers.emplace_back("L", "R", 30);
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withAddTransfer(const std::string& sourceStop, const std::string& targetStop, int duration) {
  transfers.emplace_back(sourceStop, targetStop, duration);
  return *this;
}

RaptorRouterTestBuilder& RaptorRouterTestBuilder::withSameStopTransferTime(const int time) {
  sameStopTransferTime = time;
  return *this;
}

std::shared_ptr<raptor::RaptorData> RaptorRouterTestBuilder::build() const {
  return build(routes, transfers, DAY_START_HOUR, DAY_END_HOUR, sameStopTransferTime);
}

std::shared_ptr<raptor::RaptorData> RaptorRouterTestBuilder::buildWithDefaults() {
  return this->withAddRoute1_AG()
    .withAddRoute2_HL()
    .withAddRoute3_MQ()
    .withAddRoute4_RS()
    .withAddTransfer1_ND()
    .withAddTransfer2_LR()
    .build();
}

std::shared_ptr<raptor::RaptorData> RaptorRouterTestBuilder::build(const std::vector<Route>& routes, const std::vector<Transfer>& transfers, const int dayStart, const int dayEnd, const int sameStopTransferTime) {
  raptor::RaptorRouterBuilder builder(sameStopTransferTime);
  std::unordered_set<std::string> addedStops;

  for (const auto& route : routes)
  {
    std::string routeIdF = route.id + "-F";
    std::string routeIdR = route.id + "-R";

    for (const auto& stop : route.stops | std::views::filter([&addedStops](const auto& aStop) { return !addedStops.contains(aStop); }))
    {
      builder.addStop(stop);
      addedStops.insert(stop);
    }

    builder.addRoute(routeIdF, route.stops);
    std::vector<std::string> reversedStops = route.stops;
    std::ranges::reverse(reversedStops);
    builder.addRoute(routeIdR, reversedStops);

    int tripCount = 0;
    int timestamp = dayStart * SECONDS_IN_HOUR + route.firstDepartureOffset * 60;
    while (timestamp < dayEnd * SECONDS_IN_HOUR)
    {
      std::string tripIdF = route.id + "-F-" + std::to_string(tripCount);
      std::string tripIdR = route.id + "-R-" + std::to_string(tripCount);
      builder.addTrip(tripIdF, routeIdF);
      builder.addTrip(tripIdR, routeIdR);
      tripCount++;

      int departureTimestamp = timestamp;
      int arrivalTimestamp = departureTimestamp;
      for (int i = 0; i < route.stops.size(); ++i)
      {
        if (i + 1 == route.stops.size())
        {
          departureTimestamp = arrivalTimestamp;
        }
        builder.addStopTime(routeIdF, tripIdF, i, route.stops[i], arrivalTimestamp, departureTimestamp);
        builder.addStopTime(routeIdR, tripIdR, i, reversedStops[i], arrivalTimestamp, departureTimestamp);

        arrivalTimestamp = departureTimestamp + route.travelTimeBetweenStops * 60;
        departureTimestamp = arrivalTimestamp + route.dwellTimeAtStop * 60;
      }

      timestamp += route.headWayTime * 60;
    }
  }

  std::ranges::for_each(transfers, [&builder](const Transfer& transfer) {
    builder.addTransfer(transfer.sourceStop, transfer.targetStop, transfer.duration * 60);
    builder.addTransfer(transfer.targetStop, transfer.sourceStop, transfer.duration * 60);
  });

  return builder.build();
}
