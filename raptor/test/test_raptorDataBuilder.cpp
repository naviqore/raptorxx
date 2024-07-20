//
// Created by MichaelBrunner on 20/07/2024.
//


#include "utils/RaptorDataBuilder.h"


#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

const int SECONDS_IN_HOUR = 3600;
const int DAY_START_HOUR = 5;
const int DAY_END_HOUR = 25;

const int DEFAULT_TIME_BETWEEN_STOPS = 5;
const int DEFAULT_DWELL_TIME = 1;
const int DEFAULT_HEADWAY_TIME = 15;
const int DEFAULT_OFFSET = 0;

class RaptorAlgorithm
{
public:
  // Add your RaptorAlgorithm implementation
};


struct Route
{
  string id;
  vector<string> stops;
  int firstDepartureOffset;
  int headWayTime;
  int travelTimeBetweenStops;
  int dwellTimeAtStop;

  Route(string id, vector<string> stops, int firstDepartureOffset, int headWayTime, int travelTimeBetweenStops, int dwellTimeAtStop)
    : id(move(id))
    , stops(move(stops))
    , firstDepartureOffset(firstDepartureOffset)
    , headWayTime(headWayTime)
    , travelTimeBetweenStops(travelTimeBetweenStops)
    , dwellTimeAtStop(dwellTimeAtStop) {}

  Route(string id, vector<string> stops)
    : Route(move(id), move(stops), DEFAULT_OFFSET, DEFAULT_HEADWAY_TIME, DEFAULT_TIME_BETWEEN_STOPS, DEFAULT_DWELL_TIME) {}
};

struct Transfer
{
  string sourceStop;
  string targetStop;
  int duration;

  Transfer(string sourceStop, string targetStop, int duration)
    : sourceStop(move(sourceStop))
    , targetStop(move(targetStop))
    , duration(duration) {}
};

class RaptorRouterTestBuilder
{
public:
  RaptorRouterTestBuilder()
    : sameStopTransferTime(120) {}

  RaptorRouterTestBuilder& withAddRoute1_AG() {
    return withAddRoute1_AG(DEFAULT_OFFSET, DEFAULT_HEADWAY_TIME, DEFAULT_TIME_BETWEEN_STOPS, DEFAULT_DWELL_TIME);
  }

  RaptorRouterTestBuilder& withAddRoute1_AG(int offset, int headway, int travelTime, int dwellTime) {
    return withAddRoute1_AG("R1", offset, headway, travelTime, dwellTime);
  }

  RaptorRouterTestBuilder& withAddRoute1_AG(const string& routeId, int offset, int headway, int travelTime, int dwellTime) {
    routes.emplace_back(routeId, vector<string>{"A", "B", "C", "D", "E", "F", "G"}, offset, headway, travelTime, dwellTime);
    return *this;
  }

  RaptorRouterTestBuilder& withAddRoute2_HL() {
    routes.emplace_back("R2", vector<string>{"H", "B", "I", "J", "K", "L"});
    return *this;
  }

  RaptorRouterTestBuilder& withAddRoute3_MQ() {
    routes.emplace_back("R3", vector<string>{"M", "K", "N", "O", "P", "Q"});
    return *this;
  }

  RaptorRouterTestBuilder& withAddRoute3_MQ(int offset, int headway, int travelTime, int dwellTime) {
    routes.emplace_back("R3", vector<string>{"M", "K", "N", "O", "P", "Q"}, offset, headway, travelTime, dwellTime);
    return *this;
  }

  RaptorRouterTestBuilder& withAddRoute4_RS() {
    routes.emplace_back("R4", vector<string>{"R", "P", "F", "S"});
    return *this;
  }

  RaptorRouterTestBuilder& withAddRoute5_AH_selfIntersecting() {
    routes.emplace_back("R5", vector<string>{"A", "B", "C", "D", "E", "F", "P", "O", "N", "K", "J", "I", "B", "H"});
    return *this;
  }

  RaptorRouterTestBuilder& withAddTransfer1_ND() {
    return withAddTransfer1_ND(60);
  }

  RaptorRouterTestBuilder& withAddTransfer1_ND(int duration) {
    transfers.emplace_back("N", "D", duration);
    return *this;
  }

  RaptorRouterTestBuilder& withAddTransfer2_LR() {
    transfers.emplace_back("L", "R", 30);
    return *this;
  }

  RaptorRouterTestBuilder& withAddTransfer(const string& sourceStop, const string& targetStop, int duration) {
    transfers.emplace_back(sourceStop, targetStop, duration);
    return *this;
  }

  RaptorRouterTestBuilder& withSameStopTransferTime(int time) {
    sameStopTransferTime = time;
    return *this;
  }

  std::shared_ptr<raptor::RaptorData> build() {
    return build(routes, transfers, DAY_START_HOUR, DAY_END_HOUR, sameStopTransferTime);
  }

  std::shared_ptr<raptor::RaptorData> buildWithDefaults() {
    return this->withAddRoute1_AG()
      .withAddRoute2_HL()
      .withAddRoute3_MQ()
      .withAddRoute4_RS()
      .withAddTransfer1_ND()
      .withAddTransfer2_LR()
      .build();
  }

private:
  vector<Route> routes;
  vector<Transfer> transfers;
  int sameStopTransferTime;

  static std::shared_ptr<raptor::RaptorData> build(const vector<Route>& routes, const vector<Transfer>& transfers, int dayStart, int dayEnd, int sameStopTransferTime) {
    raptor::RaptorRouterBuilder builder(sameStopTransferTime);
    unordered_set<string> addedStops;

    for (const auto& route : routes)
    {
      string routeIdF = route.id + "-F";
      string routeIdR = route.id + "-R";

      for (const auto& stop : route.stops)
      {
        if (!addedStops.contains(stop))
        {
          builder.addStop(stop);
          addedStops.insert(stop);
        }
      }

      builder.addRoute(routeIdF, route.stops);
      vector<string> reversedStops = route.stops;
      ranges::reverse(reversedStops);
      builder.addRoute(routeIdR, reversedStops);

      int tripCount = 0;
      int timestamp = dayStart * SECONDS_IN_HOUR + route.firstDepartureOffset * 60;
      while (timestamp < dayEnd * SECONDS_IN_HOUR)
      {
        string tripIdF = route.id + "-F-" + to_string(tripCount);
        string tripIdR = route.id + "-R-" + to_string(tripCount);
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

    for (const auto& transfer : transfers)
    {
      builder.addTransfer(transfer.sourceStop, transfer.targetStop, transfer.duration * 60);
      builder.addTransfer(transfer.targetStop, transfer.sourceStop, transfer.duration * 60);
    }

    return builder.build();
  }
};

TEST(RaptorRouterTest, DefaultBuild) {
  RaptorRouterTestBuilder builder;
  auto algorithm = builder.buildWithDefaults();
  // Add your assertions here
}


TEST(RaptorRouterTest, RouteBetweenTwoStopsOnSameRoute) {
  // Setup the builder and build the RaptorAlgorithm instance
  RaptorRouterTestBuilder builder;
  auto raptorData = builder.buildWithDefaults();

  // Define constants for the test
  const std::string STOP_A = "A";
  const std::string STOP_B = "B";
  const int NINE_AM = 9 * 3600; // 9 AM in seconds

  // Get the list of connections between the stops
  //  std::vector<Connection> connections = ConvenienceMethods::routeLatestDeparture(raptor, STOP_A, STOP_B, NINE_AM);

  // Assert the size of the connections list
  // ASSERT_EQ(connections.size(), 1);
}


// int main(int argc, char** argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
