//
// Created by MichaelBrunner on 20/07/2024.
//


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



TEST(RaptorAlgoTest, TestRaptorAlgo1) {
  // Setup the builder and build the RaptorAlgorithm instance
  RaptorRouterTestBuilder builder;
  const auto raptorData = builder.buildWithDefaults();

  // Define constants for the test
  const std::string STOP_A = "A";
  const std::string STOP_B = "B";
  const int NINE_AM = 9 * 3600; // 9 AM in seconds

  // Get the list of connections between the stops
  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(*raptorData);
  auto result = raptorRouter.routeEarliestArrival({{"A", 0}}, {{"B", 0}}, queryConfig);
  //  std::vector<Connection> connections = ConvenienceMethods::routeLatestDeparture(raptor, STOP_A, STOP_B, NINE_AM);

  // Assert the size of the connections list
  // ASSERT_EQ(connections.size(), 1);
}

TEST(RaptorAlgoTest, TestRaptorAlgo2) {
  // Setup the builder and build the RaptorAlgorithm instance
  RaptorRouterTestBuilder builder;
  const auto raptorData = builder.buildWithDefaults();

  // Define constants for the test
  const std::string STOP_A = "A";
  const std::string STOP_B = "Q";
  const int NINE_AM = 9 * 3600; // 9 AM in seconds

  // Get the list of connections between the stops
  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(*raptorData);
  auto result = raptorRouter.routeEarliestArrival({{"A", 0}}, {{"Q", 0}}, queryConfig);
  //  std::vector<Connection> connections = ConvenienceMethods::routeLatestDeparture(raptor, STOP_A, STOP_B, NINE_AM);

  // Assert the size of the connections list
  // ASSERT_EQ(connections.size(), 1);
}



// int main(int argc, char** argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
