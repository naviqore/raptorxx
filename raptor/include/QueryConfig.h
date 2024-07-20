//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef QUERY_CONFIG_H
#define QUERY_CONFIG_H

#include <stdexcept>
#include <limits>

class QueryConfig
{
public:
  // Default constructor
  QueryConfig()
    : maximumWalkingDuration( std::numeric_limits<int>::max())
    , minimumTransferDuration(0)
    , maximumTransferNumber( std::numeric_limits<int>::max())
    , maximumTravelTime( std::numeric_limits<int>::max()) {}

  // Parameterized constructor
  QueryConfig(int maxWalkingDuration, int minTransferDuration, int maxTransferNumber, int maxTravelTime)
    : maximumWalkingDuration(maxWalkingDuration)
    , minimumTransferDuration(minTransferDuration)
    , maximumTransferNumber(maxTransferNumber)
    , maximumTravelTime(maxTravelTime) {
    validate();
  }

  // Getters
  int getMaximumWalkingDuration() const {
    return maximumWalkingDuration;
  }
  int getMinimumTransferDuration() const {
    return minimumTransferDuration;
  }
  int getMaximumTransferNumber() const {
    return maximumTransferNumber;
  }
  int getMaximumTravelTime() const {
    return maximumTravelTime;
  }

  // Setters
  void setMaximumWalkingDuration(int maxWalkingDuration) {
    if (maxWalkingDuration < 0)
    {
      throw std::invalid_argument("Maximum walking duration must be greater than or equal to 0.");
    }
    maximumWalkingDuration = maxWalkingDuration;
  }

  void setMinimumTransferDuration(int minTransferDuration) {
    if (minTransferDuration < 0)
    {
      throw std::invalid_argument("Minimum transfer duration must be greater than or equal to 0.");
    }
    minimumTransferDuration = minTransferDuration;
  }

  void setMaximumTransferNumber(int maxTransferNumber) {
    if (maxTransferNumber < 0)
    {
      throw std::invalid_argument("Maximum transfer number must be greater than or equal to 0.");
    }
    maximumTransferNumber = maxTransferNumber;
  }

  void setMaximumTravelTime(int maxTravelTime) {
    if (maxTravelTime <= 0)
    {
      throw std::invalid_argument("Maximum travel time must be greater than 0.");
    }
    maximumTravelTime = maxTravelTime;
  }

private:
  // Validation method to check constraints
  void validate() const {
    if (maximumWalkingDuration < 0)
    {
      throw std::invalid_argument("Maximum walking duration must be greater than or equal to 0.");
    }
    if (minimumTransferDuration < 0)
    {
      throw std::invalid_argument("Minimum transfer duration must be greater than or equal to 0.");
    }
    if (maximumTransferNumber < 0)
    {
      throw std::invalid_argument("Maximum transfer number must be greater than or equal to 0.");
    }
    if (maximumTravelTime <= 0)
    {
      throw std::invalid_argument("Maximum travel time must be greater than 0.");
    }
  }

  // Member variables with default values
  int maximumWalkingDuration;
  int minimumTransferDuration;
  int maximumTransferNumber;
  int maximumTravelTime;

  // static const int INFINITY_VALUE = std::numeric_limits<int>::max();  // Use std::numeric_limits for infinity
};

#endif // QUERY_CONFIG_H
