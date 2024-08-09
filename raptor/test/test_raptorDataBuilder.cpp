//
// Created by MichaelBrunner on 20/07/2024.
//


#include "Leg.h"
#include "LocalDateTime.h"
#include "LoggerFactory.h"
#include "RaptorRouter.h"
#include "RaptorRouterTestBuilder.h"
#include "utils/RaptorDataBuilder.h"


#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <chrono>



TEST(RaptorAlgoTest, TestRaptorAlgo1) {
  // Setup the builder and build the RaptorAlgorithm instance
  RaptorRouterTestBuilder builder;
  const auto raptorData = builder.buildWithDefaults();

  // Define constants for the test
  const std::string STOP_A = "A";
  const std::string STOP_B = "B";
  constexpr int NINE_AM = 9 * 3600; // 9 AM in seconds

  // Get the list of connections between the stops
  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptorData));
  auto result = raptorRouter.routeEarliestArrival({{"A", 0}}, {{"B", 0}}, queryConfig);

  std::ranges::for_each(result, [](const auto& connection) {
    const auto legs = connection->getLegs();
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Connection: {}", connection->getLegs().size()));
  });


  //  std::vector<Connection> connections = ConvenienceMethods::routeLatestDeparture(raptor, STOP_A, STOP_B, NINE_AM);

  // Assert the size of the connections list
  // ASSERT_EQ(connections.size(), 1);
}

TEST(RaptorAlgoTest, TestRaptorAlgo2) {

  const raptor::utils::LocalDateTime START_OF_DAY{2021y, std::chrono::January, 1d, 0h, 0min, 0s};
  const raptor::utils::LocalDateTime FIVE_AM = START_OF_DAY.addHours(std::chrono::hours(5));
  const raptor::utils::LocalDateTime EIGHT_AM = START_OF_DAY.addHours(std::chrono::hours(8));
  const raptor::utils::LocalDateTime NINE_AM = START_OF_DAY.addHours(std::chrono::hours(9));

  // Setup the builder and build the RaptorAlgorithm instance
  RaptorRouterTestBuilder builder;
  const auto raptorData = builder.buildWithDefaults();

  // Define constants for the test
  const std::string STOP_A = "A";
  const std::string STOP_B = "Q";

  // Get the list of connections between the stops
  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(*raptorData);
  auto EIGHT_AM_SECONDS = static_cast<raptor::types::raptorInt>(EIGHT_AM.secondsOfDay());

  for (auto results = raptorRouter.routeEarliestArrival({{"A", EIGHT_AM_SECONDS}}, {{"Q", 0}}, queryConfig);
    const auto& connection : results)
  {
    for (const auto legs = connection->getLegs();
      const auto& leg : legs)
    {
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Route id {}", leg->getRouteId()));
    }
  }

  //  std::vector<Connection> connections = ConvenienceMethods::routeLatestDeparture(raptor, STOP_A, STOP_B, NINE_AM);

  // Assert the size of the connections list
  // ASSERT_EQ(connections.size(), 1);
}



// int main(int argc, char** argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
