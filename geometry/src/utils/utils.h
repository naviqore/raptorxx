//
// Created by MichaelBrunner on 20/06/2024.
//

#pragma once

#include <cmath>
#include "spatial/CoordinateComponent.h"
#include <stdexcept>
#include <numbers>

namespace geometry::utils {

  double distance(const CoordinateComponentConcept auto& a, const CoordinateComponentConcept auto& b) {
    const double dx = a.getFirstCoordinate() - b.getFirstCoordinate();
    const double dy = a.getSecondCoordinate() - b.getSecondCoordinate();
    return std::sqrt(dx * dx + dy * dy);
  }

  template<CoordinateComponentConcept T>
  double getCoordinateComponent(const T& point, const int axis) {
    if (axis == 0)
    {
      return point.getFirstCoordinate();
    }
    if (axis == 1)
    {
      return point.getSecondCoordinate();
    }
    throw std::invalid_argument("Invalid axis value");
  }

  double haversineDistance(const CoordinateComponentConcept auto& aFirstPoint, const CoordinateComponentConcept auto& aSecondPoint) {
    constexpr double kEarthRadiusKilometers = 6'371.0;
    auto toRadians = [](const double degrees) { return degrees * std::numbers::pi / 180.0; };

    const double lat1 = toRadians(aFirstPoint.getFirstCoordinate());
    const double lon1 = toRadians(aFirstPoint.getSecondCoordinate());

    const double lat2 = toRadians(aSecondPoint.getFirstCoordinate());
    const double lon2 = toRadians(aSecondPoint.getSecondCoordinate());

    const double a = std::sin((lat2 - lat1) / 2) * std::sin((lat2 - lat1) / 2) + std::cos(lat1) * std::cos(lat2) * std::sin((lon2 - lon1) / 2) * std::sin((lon2 - lon1) / 2);

    const double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return kEarthRadiusKilometers * c;
  }


}
