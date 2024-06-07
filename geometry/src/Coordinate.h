//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef COORDINATE_H
#define COORDINATE_H

namespace geometry {

  template<typename T>
    requires std::convertible_to<T, double>
  class Coordinate
  {
    T value;
  public:
    explicit Coordinate(const T value)
      : value(value) {}

    [[nodiscard]] T getValue() const {
      return value;
    }
  };

} // geometry

#endif //COORDINATE_H
