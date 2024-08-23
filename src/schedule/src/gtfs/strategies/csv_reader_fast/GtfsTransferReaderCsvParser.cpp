//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsTransferReaderCsvParser.h"

#include "csv_wrapper.h"


namespace schedule::gtfs {


  struct TempTransfer
  {
    std::string fromStopId;
    std::string toStopId;
    std::string transferType;
    std::string minTransferTime;
  };

  GtfsTransferReaderCsvParser::GtfsTransferReaderCsvParser(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsTransferReaderCsvParser::GtfsTransferReaderCsvParser(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsTransferReaderCsvParser::operator()(GtfsReader& aReader) const {
    auto reader = io::CSVReader<4, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
    reader.set_header("from_stop_id", "to_stop_id", "transfer_type", "min_transfer_time");

    std::string fromStopId;
    std::string toStopId;
    int transferType;
    int minTransferTime;
    reader.read_header(io::ignore_extra_column, "from_stop_id", "to_stop_id", "transfer_type", "min_transfer_time");
    while (reader.read_row(fromStopId, toStopId, transferType, minTransferTime))
    {

      auto temp = fromStopId;

      aReader.getData().get().transfer[temp].emplace_back(
        std::move(fromStopId),
        std::move(toStopId),
        static_cast<Transfer::TransferType>(transferType),
        minTransferTime);
    }
  }
} // gtfs
  // schedule