//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

namespace raptor::config {
  class WalkingConfig
  {
  public:
    explicit WalkingConfig(int const maxWalkingDuration)
      : maxWalkingDuration(maxWalkingDuration) {
    }

    [[nodiscard]] int getMaxWalkingDuration() const {
      return maxWalkingDuration;
    }

    void setMaxWalkingDuration(int const maxWalkingDuration) {
      this->maxWalkingDuration = maxWalkingDuration;
    }

    int operator()() const {
      return maxWalkingDuration;
    }

  private:
    int maxWalkingDuration;
  };
}
