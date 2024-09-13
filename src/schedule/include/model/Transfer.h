//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef TRANSFER_H
#define TRANSFER_H

#include <string>
#include <stdexcept>
#include <cstdint>
#include <schedule_export.h>

// https://gtfs.org/schedule/reference/#transferstxt
namespace schedule::gtfs {
  struct SCHEDULE_API Transfer
  {

    enum TransferType : uint8_t
    {
      RECOMMENDED,
      TIMED,
      MINIMUM_TIME,
      NOT_POSSIBLE
    };

    Transfer(std::string aFromStopId, std::string&& aToStopId, TransferType const aTransferType, const int minTransferTime)
      : fromStopId(std::move(aFromStopId))
      , toStopId(std::move(aToStopId))
      , transferType(aTransferType)
      , minTransferTime(minTransferTime) {
      if (fromStopId.empty()
          || toStopId.empty())
      {
        throw std::invalid_argument("Mandatory transfer fields must not be empty");
      }
    }
    std::string fromStopId;
    std::string toStopId;
    TransferType transferType;
    int minTransferTime = 120;
  };

  inline auto transferHash = [](const Transfer& transfer) {
    const auto h1 = std::hash<std::string>{}(transfer.fromStopId);
    const auto h2 = std::hash<std::string>{}(transfer.toStopId);
    return h1 ^ (h2 << 1);
  };

  inline auto transferEqual = [](const Transfer& lhs, const Transfer& rhs) {
    return lhs.fromStopId == rhs.fromStopId && lhs.toStopId == rhs.toStopId;
  };

} // gtfs

#endif //TRANSFER_H
