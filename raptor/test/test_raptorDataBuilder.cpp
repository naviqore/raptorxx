//
// Created by MichaelBrunner on 20/07/2024.
//


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

  using namespace std::chrono_literals;
  auto specific_time = std::chrono::local_days{2024y / 8 / 2d} + 12h + 30min; // 2024-08-02 12:30:00

  // TODO investigate chrono library
  auto later = specific_time + std::chrono::hours(2) + std::chrono::minutes(45); // 2024-08-02 15:15:00

  auto tp1 = std::chrono::local_days{std::chrono::year{2024} / std::chrono::August / 2} + 10h + 30min;
  auto tp2 = std::chrono::local_days{std::chrono::year{2024} / std::chrono::August / 2} + 12h + 45min;


  constexpr int START_OF_DAY = 0;                   // 00:00 in seconds
  constexpr int FIVE_AM = START_OF_DAY + 5 * 3600;  // 5 AM in seconds
  constexpr int EIGHT_AM = START_OF_DAY + 8 * 3600; // 8 AM in seconds
  constexpr int NINE_AM = START_OF_DAY + 9 * 3600;  // 9 AM in seconds
  // Setup the builder and build the RaptorAlgorithm instance
  RaptorRouterTestBuilder builder;
  const auto raptorData = builder.buildWithDefaults();

  // Define constants for the test
  const std::string STOP_A = "A";
  const std::string STOP_B = "Q";

  // Get the list of connections between the stops
  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(*raptorData);
  auto result = raptorRouter.routeEarliestArrival({{"A", NINE_AM}}, {{"Q", 0}}, queryConfig);
  //  std::vector<Connection> connections = ConvenienceMethods::routeLatestDeparture(raptor, STOP_A, STOP_B, NINE_AM);

  // Assert the size of the connections list
  // ASSERT_EQ(connections.size(), 1);
}



// int main(int argc, char** argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
