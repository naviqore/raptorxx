//
// Created by MichaelBrunner on 20/07/2024.
//


#include "Leg.h"
#include "LocalDateTime.h"
#include "LoggerFactory.h"
#include "RaptorRouter.h"
#include "RaptorRouterTestBuilder.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <chrono>


class RaptorAlgoTest : public ::testing::Test
{
protected:
  std::unique_ptr<RaptorRouterTestBuilder> builder;

  raptor::utils::LocalDateTime START_OF_DAY{raptor::utils::LocalDateTime{2021y, std::chrono::January, 1d, 0h, 0min, 0s}};

  raptor::utils::LocalDateTime FIVE_AM;
  raptor::utils::LocalDateTime EIGHT_AM;
  raptor::utils::LocalDateTime NINE_AM;

  void SetUp() override {
    builder = std::make_unique<RaptorRouterTestBuilder>();

    FIVE_AM = START_OF_DAY.addHours(std::chrono::hours(5));
    EIGHT_AM = START_OF_DAY.addHours(std::chrono::hours(8));
    NINE_AM = START_OF_DAY.addHours(std::chrono::hours(9));
  }

  void TearDown() override {
    builder.reset();
  }
};

TEST_F(RaptorAlgoTest, TestRaptorAlgo1) {

  // Arrange
  const auto raptorData = builder->buildWithDefaults();

  const std::string STOP_A = "A";
  const std::string STOP_B = "B";

  // Get the list of connections between the stops
  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptorData));
  auto NINE_AM_SECONDS = static_cast<raptor::types::raptorInt>(START_OF_DAY.secondsOfDay());
  // Act
  const auto connections = raptorRouter.routeEarliestArrival({{"A", NINE_AM_SECONDS}}, {{"B", 0}}, queryConfig);

  // Assert
  const auto legs = connections[0]->getLegs();
  const auto& leg = legs[0];
  const auto departureTime = raptor::utils::fromSecondsOfToday(leg->getDepartureTime());
  const auto arrivalTime = raptor::utils::fromSecondsOfToday(leg->getArrivalTime());

  ASSERT_EQ(connections.size(), 1);
  ASSERT_EQ(legs.size(), 1);
  ASSERT_EQ(leg->getRouteId(), "R1-F");
  ASSERT_EQ(leg->getTripId(), "R1-F-0");
  ASSERT_EQ(departureTime.secondsOfDay(), 18'000);
  ASSERT_EQ(arrivalTime.secondsOfDay(), 18'300);
}

TEST_F(RaptorAlgoTest, TestRaptorAlgo2) {

  // Arrange
  const auto raptorData = builder->buildWithDefaults();

  const std::string STOP_A = "A";
  const std::string STOP_B = "Q";

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(*raptorData);
  auto EIGHT_AM_SECONDS = static_cast<raptor::types::raptorInt>(EIGHT_AM.secondsOfDay());

  // Act
  const auto connections = raptorRouter.routeEarliestArrival({{"A", EIGHT_AM_SECONDS}}, {{"Q", 0}}, queryConfig);

  // Assert
  ASSERT_EQ(connections.size(), 2);

  ///////////////////// test first connection /////////////////////
  ///
  {
    const auto& connection1 = connections[0];

    ASSERT_EQ(connection1->getLegs().size(), 3);

    const auto leg1 = connection1->getLegs()[0];
    ASSERT_EQ(leg1->getRouteId(), "R1-F");
    ASSERT_EQ(leg1->getTripId(), "R1-F-12");
    ASSERT_EQ(leg1->getFromStopId(), "A");
    ASSERT_EQ(leg1->getToStopId(), "D");
    ASSERT_EQ(leg1->getDepartureTime(), 28'800);
    ASSERT_EQ(leg1->getArrivalTime(), 29'820);
    ASSERT_TRUE(leg1->getType().value() == raptor::Leg::Type::ROUTE);

    const auto leg2 = connection1->getLegs()[1];
    ASSERT_EQ(leg2->getRouteId(), "transfer_D_N");
    ASSERT_EQ(leg2->getTripId(), "");
    ASSERT_EQ(leg2->getFromStopId(), "D");
    ASSERT_EQ(leg2->getToStopId(), "N");
    ASSERT_EQ(leg2->getDepartureTime(), 29'820);
    ASSERT_EQ(leg2->getArrivalTime(), 33'420);
    ASSERT_TRUE(leg2->getType().value() == raptor::Leg::Type::WALK_TRANSFER);

    const auto leg3 = connection1->getLegs()[2];
    ASSERT_EQ(leg3->getRouteId(), "R3-F");
    ASSERT_EQ(leg3->getTripId(), "R3-F-17");
    ASSERT_EQ(leg3->getFromStopId(), "N");
    ASSERT_EQ(leg3->getToStopId(), "Q");
    ASSERT_EQ(leg3->getDepartureTime(), 34'020);
    ASSERT_EQ(leg3->getArrivalTime(), 35'040);
    ASSERT_EQ(leg3->getType().value(), raptor::Leg::Type::ROUTE);
  }

  ///////////////////// test second connection /////////////////////
  ///

  {
    const auto& connection2 = connections[1];

    ASSERT_EQ(connection2->getLegs().size(), 3);

    const auto leg1 = connection2->getLegs()[0];
    ASSERT_EQ(leg1->getRouteId(), "R1-F");
    ASSERT_EQ(leg1->getTripId(), "R1-F-12");
    ASSERT_EQ(leg1->getFromStopId(), "A");
    ASSERT_EQ(leg1->getToStopId(), "F");
    ASSERT_EQ(leg1->getDepartureTime(), 28'800);
    ASSERT_EQ(leg1->getArrivalTime(), 30'540);
    ASSERT_TRUE(leg1->getType().value() == raptor::Leg::Type::ROUTE);

    const auto leg2 = connection2->getLegs()[1];
    ASSERT_EQ(leg2->getRouteId(), "R4-R");
    ASSERT_EQ(leg2->getTripId(), "R4-R-14");
    ASSERT_EQ(leg2->getFromStopId(), "F");
    ASSERT_EQ(leg2->getToStopId(), "P");
    ASSERT_EQ(leg2->getDepartureTime(), 30'960);
    ASSERT_EQ(leg2->getArrivalTime(), 31'260);
    ASSERT_TRUE(leg2->getType().value() == raptor::Leg::Type::ROUTE);

    const auto leg3 = connection2->getLegs()[2];
    ASSERT_EQ(leg3->getRouteId(), "R3-F");
    ASSERT_EQ(leg3->getTripId(), "R3-F-14");
    ASSERT_EQ(leg3->getFromStopId(), "P");
    ASSERT_EQ(leg3->getToStopId(), "Q");
    ASSERT_EQ(leg3->getDepartureTime(), 32'040);
    ASSERT_EQ(leg3->getArrivalTime(), 32'340);
    ASSERT_EQ(leg3->getType().value(), raptor::Leg::Type::ROUTE);
  }
}
