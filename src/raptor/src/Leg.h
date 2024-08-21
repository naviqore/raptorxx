//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef LEG_H
#define LEG_H

#include <optional>
#include <string>

namespace raptor {
  class Leg
  {
  public:
    enum Type
    {
      WALK_TRANSFER,
      ROUTE
    };

    virtual ~Leg() = default;

    [[nodiscard]] virtual std::string const& getRouteId() const = 0;

    [[nodiscard]] virtual std::string const& getTripId() const = 0;

    [[nodiscard]] virtual std::string const& getFromStopId() const = 0;

    [[nodiscard]] virtual std::string const& getToStopId() const = 0;

    [[nodiscard]] virtual int getDepartureTime() const = 0;

    [[nodiscard]] virtual int getArrivalTime() const = 0;

    [[nodiscard]] virtual std::optional<Type> getType() const = 0;

    bool operator<(const Leg& other) const {
      if (getDepartureTime() != other.getDepartureTime())
      {
        return getDepartureTime() < other.getDepartureTime();
      }
      return getArrivalTime() < other.getArrivalTime();
    }
  };

}

#endif //LEG_H
