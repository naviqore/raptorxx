//
// Created by MichaelBrunner on 20/06/2024.
//

#include <gtest/gtest.h>
#include "LoggerFactory.h"
#include "utils.h"
#include "usings.h"

#include <ranges>
#include <unordered_map>


TEST(geomety, TestK2dTree)
{
  auto kdTree = geometry::kd_tree::spatialK2dTree({});
  const auto firstCoordinate = geometry::kd_tree::spatialCoordinate(1.0);
  const auto secondCoordinate = geometry::kd_tree::spatialCoordinate(2.0);
  const auto coordiante = geometry::kd_tree::coordinateComponent(firstCoordinate, secondCoordinate);

  EXPECT_THROW(kdTree.nearest(coordiante), std::runtime_error);
}


class TestK2dTree : public testing::Test {
  using hasher = geometry::utils::CoordinateComponentHash<geometry::kd_tree::spatialCoordinate>;
  using equalizer = geometry::utils::CoordinateComponentEqual<geometry::kd_tree::spatialCoordinate>;

protected:
  std::unordered_map<geometry::kd_tree::coordinateComponent, std::string, hasher, equalizer> stations;
  std::unique_ptr<geometry::kd_tree::spatialK2dTree> kdTree;

  void SetUp() override
  {
    // arrange
    getConsoleLogger(LoggerName::GEOMETRY)->info("TestK2dTree::SetUp");
    stations[geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.4230127859742), geometry::kd_tree::spatialCoordinate(9.369479194531577))] = "St. Gallen Train Station";
    stations[geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.41190313108875), geometry::kd_tree::spatialCoordinate(9.25303543394843))] = "Gossau Train Station";
    stations[geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.41549705750434), geometry::kd_tree::spatialCoordinate(9.18936624689178))] = "Flawil Train Station";
    stations[geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.46216381226957), geometry::kd_tree::spatialCoordinate(9.040973103320258))] = "Wil Train Station";
    stations[geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.5002860698031), geometry::kd_tree::spatialCoordinate(8.723829255246654))] = "Wintherthur Train Station";

    std::vector<geometry::kd_tree::coordinateComponent> coordinates;
    for (const auto& coordinate : stations | std::views::keys) {
      coordinates.push_back(coordinate);
    }

    kdTree = std::make_unique<geometry::kd_tree::spatialK2dTree>(coordinates);
  }
};

TEST_F(TestK2dTree, TestK2dTreeStGallenToWinterthur)
{
  // act
  const auto findNearestFromRickenbach = geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.53556052653995), geometry::kd_tree::spatialCoordinate(8.789113533722448));
  const auto findNeaerestFromKreuzbleiche = geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.419620275547636), geometry::kd_tree::spatialCoordinate(9.359281921519944));

  const auto nearestRickenbach = kdTree->nearest(findNearestFromRickenbach);
  getConsoleLogger(LoggerName::GEOMETRY)->info(std::format("Nearest to Rickenbach is: {}", stations[nearestRickenbach]));

  const auto neaerestKreuzbleiche = kdTree->nearest(findNeaerestFromKreuzbleiche);
  getConsoleLogger(LoggerName::GEOMETRY)->info(std::format("Nearest to Kreuzbleiche is: {}", stations[neaerestKreuzbleiche]));

  // assert
  ASSERT_TRUE(stations[nearestRickenbach] == "Wintherthur Train Station");
  ASSERT_TRUE(stations[neaerestKreuzbleiche] == "St. Gallen Train Station");
}

TEST_F(TestK2dTree, TestK2dTreeRangeSearch)
{
  // act
  const auto findNeaerestFromKreuzbleiche = geometry::kd_tree::coordinateComponent(geometry::kd_tree::spatialCoordinate(47.419620275547636), geometry::kd_tree::spatialCoordinate(9.359281921519944));

  constexpr auto rangeInKiloMeters = 30.0_km;
  const auto nearestRickenbach = kdTree->rangeSearch(findNeaerestFromKreuzbleiche, rangeInKiloMeters);

  // assert
  for (auto it = nearestRickenbach.begin(); it != nearestRickenbach.end(); ++it) {
    getConsoleLogger(LoggerName::GEOMETRY)->info(std::format("Nearest to Kreuzbleiche is: {}", stations[*it]));
    switch (std::distance(nearestRickenbach.begin(), it)) {
      case 0:
        ASSERT_TRUE(stations[*it] == "St. Gallen Train Station");
        break;
      case 1:
        ASSERT_TRUE(stations[*it] == "Gossau Train Station");
        break;
      case 2:
        ASSERT_TRUE(stations[*it] == "Flawil Train Station");
        break;
      case 3:
        ASSERT_TRUE(stations[*it] == "Wil Train Station");
        break;
      default:
        break;
    }
  }
}
