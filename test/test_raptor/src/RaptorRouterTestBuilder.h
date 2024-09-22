//
// Created by MichaelBrunner on 20/07/2024.
//

#pragma once

#include "RaptorRouterBuilder.h"
#include <vector>
#include <string>
#include <memory>
#include <ranges>

constexpr int SECONDS_IN_HOUR = 3600;
constexpr int DAY_START_HOUR = 5;
constexpr int DAY_END_HOUR = 25;

constexpr int DEFAULT_TIME_BETWEEN_STOPS = 5;
constexpr int DEFAULT_DWELL_TIME = 1;
constexpr int DEFAULT_HEADWAY_TIME = 15;
constexpr int DEFAULT_OFFSET = 0;

struct Route
{
  std::string id;
  std::vector<std::string> stops;
  int firstDepartureOffset;
  int headWayTime;
  int travelTimeBetweenStops;
  int dwellTimeAtStop;

  Route(std::string id, std::vector<std::string> stops, int firstDepartureOffset, int headWayTime, int travelTimeBetweenStops, int dwellTimeAtStop);
  Route(std::string id, std::vector<std::string> stops);
};

struct Transfer
{
  std::string sourceStop;
  std::string targetStop;
  int duration;

  Transfer(std::string sourceStop, std::string targetStop, int duration);
};

class RaptorRouterTestBuilder
{
public:
  RaptorRouterTestBuilder();

  RaptorRouterTestBuilder& withAddRoute1_AG();
  RaptorRouterTestBuilder& withAddRoute1_AG(int offset, int headway, int travelTime, int dwellTime);
  RaptorRouterTestBuilder& withAddRoute1_AG(const std::string& routeId, int offset, int headway, int travelTime, int dwellTime);

  RaptorRouterTestBuilder& withAddRoute2_HL();
  RaptorRouterTestBuilder& withAddRoute3_MQ();
  RaptorRouterTestBuilder& withAddRoute3_MQ(int offset, int headway, int travelTime, int dwellTime);
  RaptorRouterTestBuilder& withAddRoute4_RS();
  RaptorRouterTestBuilder& withAddRoute5_AH_selfIntersecting();

  RaptorRouterTestBuilder& withAddTransfer1_ND();
  RaptorRouterTestBuilder& withAddTransfer1_ND(int duration);
  RaptorRouterTestBuilder& withAddTransfer2_LR();
  RaptorRouterTestBuilder& withAddTransfer(const std::string& sourceStop, const std::string& targetStop, int duration);

  RaptorRouterTestBuilder& withSameStopTransferTime(int time);

  [[nodiscard]] std::shared_ptr<raptor::RaptorData> build() const;
  std::shared_ptr<raptor::RaptorData> buildWithDefaults();

private:
  std::vector<Route> routes;
  std::vector<Transfer> transfers;
  int sameStopTransferTime;

  static std::shared_ptr<raptor::RaptorData> build(const std::vector<Route>& routes, const std::vector<Transfer>& transfers, int dayStart, int dayEnd, int sameStopTransferTime);
};

