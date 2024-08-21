//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef LEGIMPL_H
#define LEGIMPL_H

#include "Leg.h"
#include "usingTypes.h"


#include <optional>

namespace raptor {

  class LegImpl final : public Leg
  {

    std::string routeId;
    std::string tripId;
    std::string fromStopId;
    std::string toStopId;
    types::raptorIdx departureTime;
    types::raptorIdx arrivalTime;
    std::optional<Type> type = std::nullopt;

  public:
    explicit LegImpl(std::string routeId, std::string tripId, std::string fromStopId, std::string toStopId, types::raptorIdx departureTime, types::raptorIdx arrivalTime, Type type);

    ~LegImpl() override = default;

    [[nodiscard]] std::string const& getRouteId() const override;

    [[nodiscard]] std::string const& getTripId() const override;

    [[nodiscard]] std::string const& getFromStopId() const override;

    [[nodiscard]] std::string const& getToStopId() const override;

    [[nodiscard]] int getDepartureTime() const override;

    [[nodiscard]] int getArrivalTime() const override;

    [[nodiscard]] std::optional<Type> getType() const override;
  };

} // raptor

#endif //LEGIMPL_H
