//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsTransferReaderCsv.h"

#include "GtfsCsvHelpers.h"
#include "src/utils/utils.h"


namespace schedule::gtfs {


  struct TempTransfer
  {
    std::string fromStopId;
    std::string toStopId;
    std::string transferType;
    std::string minTransferTime;
  };

  GtfsTransferReaderCsv::GtfsTransferReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsTransferReaderCsv::GtfsTransferReaderCsv(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsTransferReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader();
    if (!reader.mmap(filename))
    {
      throw std::runtime_error("Cannot read file");
    }
    const auto header = reader.header();
    const std::vector<std::string> headerItems = utils::mapHeaderItemsToVector(header);

    std::map<size_t, std::string> headerMap = utils::createHeaderMap(headerItems);

    int index = 0;
    for (const auto& row : reader)
    {
      TempTransfer tempStop;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());
        value = utils::removeUtf8Bom(value);
        value = utils::removeQuotesFromStringView(value);

        static const std::map<std::string, std::function<void(TempTransfer&, const std::string&)>> columnActions = {
          {"from_stop_id", [](TempTransfer& transfer, const std::string& val) { transfer.fromStopId = val; }},
          {"to_stop_id", [](TempTransfer& transfer, const std::string& val) { transfer.toStopId = val; }},
          {"transfer_type", [](TempTransfer& transfer, const std::string& val) { transfer.transferType = val; }},
          {"min_transfer_time", [](TempTransfer& transfer, const std::string& val) { transfer.minTransferTime = val; }},
        };

        if (columnActions.contains(columnName))
        {
          columnActions.at(columnName)(tempStop, value);
        }
        ++index;
      }
      if (!tempStop.fromStopId.empty())
      {
        auto fromStopId = tempStop.fromStopId;

        aReader.getData().get().transferFrom[fromStopId].emplace_back(
          std::move(tempStop.fromStopId),
          std::move(tempStop.toStopId),
          static_cast<Transfer::TransferType>(std::stoi(tempStop.transferType)),
          std::stoi(tempStop.minTransferTime));
      }
    }
  }
} // gtfs
  // schedule