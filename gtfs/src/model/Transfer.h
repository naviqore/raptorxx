//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef TRANSFER_H
#define TRANSFER_H

#include <string>
#include <stdexcept>

// https://gtfs.org/schedule/reference/#transferstxt
namespace gtfs {
  struct Transfer
  {
    Transfer(std::string&& aFromStopId, std::string&& aToStopId, int aTransferType)
      : fromStopId(std::move(aFromStopId))
      , toStopId(std::move(aToStopId))
      , transferType(aTransferType) {
      if (fromStopId.empty()
          || toStopId.empty())
      {
        throw std::invalid_argument("Mandatory transfer fields must not be empty");
      }
    }
    std::string fromStopId;
    std::string toStopId;
    int transferType;
  };

} // gtfs

#endif //TRANSFER_H
