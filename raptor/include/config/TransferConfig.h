//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

namespace raptor::config {
  class TransferConfig
  {
  public:
    explicit TransferConfig(int const minTransferDuration, int const maxTransferNumber)
      : minTransferDuration(minTransferDuration)
      , maxTransferNumber(maxTransferNumber) {
    }

    [[nodiscard]] int getMinTransferDuration() const {
      return minTransferDuration;
    }
    [[nodiscard]] int getMaxTransferNumber() const {
      return maxTransferNumber;
    }
    void setMinTransferDuration(int const minTransferDuration) {
      this->minTransferDuration = minTransferDuration;
    }
    void setMaxTransferNumber(int const maxTransferNumber) {
      this->maxTransferNumber = maxTransferNumber;
    }

  private:
    int minTransferDuration;
    int maxTransferNumber;
  };
}
