//
// Created by MichaelBrunner on 20/07/2024.
//

#include <raptorRouteStructures.h>
#include <RouteBuilder.h>
#include <gtest/gtest.h>


class RouteBuilderTest : public ::testing::Test
{
protected:
  static const std::string ROUTE_1;
  static const std::string TRIP_1;
  static const std::string STOP_1;
  static const std::string STOP_2;
  static const std::string STOP_3;

  std::unique_ptr<raptor::RouteBuilder> builder;

  void SetUp() override {
    builder = std::make_unique<raptor::RouteBuilder>(ROUTE_1, std::vector{STOP_1, STOP_2, STOP_3});
    builder->addTrip(TRIP_1);
  }

  void TearDown() override {
    builder.reset();
  }
};

const std::string RouteBuilderTest::ROUTE_1 = "route1";
const std::string RouteBuilderTest::TRIP_1 = "trip1";
const std::string RouteBuilderTest::STOP_1 = "stop1";
const std::string RouteBuilderTest::STOP_2 = "stop2";
const std::string RouteBuilderTest::STOP_3 = "stop3";

TEST_F(RouteBuilderTest, ShouldAddValidTrips) {
  EXPECT_NO_THROW(builder->addTrip("trip2"));
}

TEST_F(RouteBuilderTest, ShouldNotAddDuplicateTrip) {
  EXPECT_THROW(builder->addTrip(TRIP_1), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldAddValidStopTimes) {
  EXPECT_NO_THROW(builder->addStopTime(TRIP_1, 0, STOP_1, raptor::StopTime(100, 200)));
  EXPECT_NO_THROW(builder->addStopTime(TRIP_1, 1, STOP_2, raptor::StopTime(300, 400)));
  EXPECT_NO_THROW(builder->addStopTime(TRIP_1, 2, STOP_3, raptor::StopTime(500, 600)));
}

TEST_F(RouteBuilderTest, ShouldNotAddStopTimeToNonExistentTrip) {
  EXPECT_THROW(builder->addStopTime("nonexistentTrip", 0, STOP_1, raptor::StopTime(100, 200)), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldNotAddStopTimeWithNegativePosition) {
  EXPECT_THROW(builder->addStopTime(TRIP_1, -1, STOP_1, raptor::StopTime(100, 200)), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldNotAddStopTimeWithPositionOutOfBounds) {
  EXPECT_THROW(builder->addStopTime(TRIP_1, 3, STOP_1, raptor::StopTime(100, 200)), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldNotAddStopTimeForNonMatchingStop) {
  EXPECT_THROW(builder->addStopTime(TRIP_1, 0, "nonexistentStop", raptor::StopTime(100, 200)), std::invalid_argument);
  EXPECT_THROW(builder->addStopTime(TRIP_1, 0, STOP_2, raptor::StopTime(100, 200)), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldNotAddDuplicateStopTimes) {
  builder->addStopTime(TRIP_1, 0, STOP_1, raptor::StopTime(100, 200));
  EXPECT_THROW(builder->addStopTime(TRIP_1, 0, STOP_1, raptor::StopTime(300, 400)), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldNotAddStopTimeWithInvalidTimes) {
  builder->addStopTime(TRIP_1, 0, STOP_1, raptor::StopTime(100, 200));
  EXPECT_THROW(builder->addStopTime(TRIP_1, 1, STOP_2, raptor::StopTime(150, 300)), std::invalid_argument);
  builder->addStopTime(TRIP_1, 1, STOP_2, raptor::StopTime(300, 400));
  EXPECT_THROW(builder->addStopTime(TRIP_1, 2, STOP_3, raptor::StopTime(350, 600)), std::invalid_argument);
}

TEST_F(RouteBuilderTest, ShouldBuildRoute) {
  builder->addStopTime(TRIP_1, 0, STOP_1, raptor::StopTime(100, 200));
  builder->addStopTime(TRIP_1, 1, STOP_2, raptor::StopTime(300, 400));
  builder->addStopTime(TRIP_1, 2, STOP_3, raptor::StopTime(500, 600));
  EXPECT_NO_THROW(builder->build());
}
