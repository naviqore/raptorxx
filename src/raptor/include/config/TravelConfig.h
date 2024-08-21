//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

namespace raptor::config {
  class TravelConfig
  {
  public:
    explicit TravelConfig(int const maxTravelTime)
      : maximumTravelTime(maxTravelTime) {
    }

    [[nodiscard]] int getMaxTravelTime() const {
      return maximumTravelTime;
    }

    void setMaxTravelTime(int const maxTravelTime) {
      this->maximumTravelTime = maxTravelTime;
    }

    int operator()() const {
      return maximumTravelTime;
    }

  private:
    int maximumTravelTime;
  };
}
