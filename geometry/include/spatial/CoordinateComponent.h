//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef BASEPOINT_H
#define BASEPOINT_H

#include <concepts>
#include <cmath>


template<typename T>
concept CoordinateItemConcept = requires(T a) {
  { a.getValue() } -> std::convertible_to<double>;
};

template<typename T>
concept CoordinateComponentConcept = requires(T a) {
  { a.getFirstCoordinate() } -> std::convertible_to<double>;
  { a.getSecondCoordinate() } -> std::convertible_to<double>;
};

namespace geometry {

  template<CoordinateItemConcept T>
  class CoordinateComponent
  {
    T firstCoordinate;
    T secondCoordinate;

    static constexpr double EPSILON = 1E-6;

    bool isEqual(CoordinateComponent const& other) {
      return std::abs(getFirstCoordinate() - other.getFirstCoordinate()) < EPSILON
             && std::abs(getSecondCoordinate() - other.getSecondCoordinate()) < EPSILON;
    }


  public:
    explicit CoordinateComponent(T first, T second)
      : firstCoordinate(first)
      , secondCoordinate(second) {}

    [[nodiscard]] double getFirstCoordinate() const {
      return firstCoordinate.getValue();
    }
    [[nodiscard]] double getSecondCoordinate() const {
      return secondCoordinate.getValue();
    }

    bool operator==(const CoordinateComponent& other) const {
      return isEqual(other);
    }

    bool operator!=(const CoordinateComponent& other) const {
      return !isEqual(other);
    }

    CoordinateComponent operator+=(const CoordinateComponent& other) {
      return Point(getFirstCoordinate() + other.getFirstCoordinate(),
                   getSecondCoordinate() + other.getSecondCoordinate());
    }

    CoordinateComponent operator-(const CoordinateComponent& other) {
      return Point(getFirstCoordinate() - other.getFirstCoordinate(),
                   getSecondCoordinate() - other.getSecondCoordinate());
    }

    CoordinateComponent operator*(const double scalar) {
      return CoordinateComponent(getFirstCoordinate() * scalar,
                                 getSecondCoordinate() * scalar);
    }

    CoordinateComponent operator/(const double scalar) {
      return CoordinateComponent(getFirstCoordinate() / scalar,
                                 getSecondCoordinate() / scalar);
    }

    double distanceTo(const CoordinateComponent& other) {
      return std::sqrt(distanceToSquared(other));
    }

    double distanceToSquared(const CoordinateComponent& other) {
      return std::pow(getFirstCoordinate() - other.getFirstCoordinate(), 2)
             + std::pow(getSecondCoordinate() - other.getSecondCoordinate(), 2);
    }

    CoordinateComponent vectorTo(const CoordinateComponent& other) {
      return other - *this;
    }

    CoordinateComponent normalize() {
      return *this / distanceTo(CoordinateComponent(0, 0));
    }
  };

} // geometry

#endif //BASEPOINT_H
