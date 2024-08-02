//
// Created by MichaelBrunner on 02/08/2024.
//

#pragma once
#include "Connection.h"
#include "Leg.h"
#include "usingTypes.h"


#include <vector>

namespace raptor {

  class RaptorConnection final : public Connection
  {
    std::vector<std::shared_ptr<Leg>> legs;

    static void validateLegOrder(const std::shared_ptr<Leg>& current, const std::shared_ptr<Leg>& next);

  public:
    RaptorConnection() = default;

    void addLeg(const std::shared_ptr<Leg>& leg);
    void initialize();

    [[nodiscard]] int getDepartureTime() const override;

    [[nodiscard]] int getArrivalTime() const override;

    [[nodiscard]] std::string getFromStopId() const override;

    [[nodiscard]] std::string getToStopId() const override;

    [[nodiscard]] int getDurationInSeconds() const override;

    [[nodiscard]] std::vector<std::shared_ptr<Leg>> getWalkTransfers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<Leg>> getRouteLegs() const override;

    [[nodiscard]] types::raptorInt getNumberOfSameStopTransfers() const override;

    [[nodiscard]] types::raptorInt getNumberOfTotalTransfers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<Leg>> getLegs() const override;
  };

} // raptor
