//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsStopTimeReaderCsvParser.h"

#include "csv_wrapper.h"

#include <ranges>

namespace schedule::gtfs {

  struct TempStopTime {
    std::string stopId;
    std::string tripId;
    std::string arrivalTime;
    std::string departureTime;
    std::string stopSequence;
  };

  GtfsStopTimeReaderCsvParser::GtfsStopTimeReaderCsvParser(std::string&& filename)
    : filename(std::move(filename))
  {
    if (this->filename.empty()) {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsStopTimeReaderCsvParser::GtfsStopTimeReaderCsvParser(std::string const& filename)
    : filename(filename)
  {
    if (this->filename.empty()) {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsStopTimeReaderCsvParser::operator()(GtfsReader& aReader) const
  {
    auto reader = io::CSVReader<7, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
    reader.set_header("trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence", "pickup_type", "drop_off_type");

    std::string stopId;
    std::string tripId;
    std::string arrivalTime;
    std::string departureTime;
    int stopSequence;
    std::string pickupType;
    std::string dropOffType;
    reader.read_header(io::ignore_extra_column, "trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence", "pickup_type", "drop_off_type");
    while (reader.read_row(tripId, arrivalTime, departureTime, stopId, stopSequence, pickupType, dropOffType)) {
      std::ignore = pickupType;
      std::ignore = dropOffType;

      aReader.getData().get().stopTimes[stopId].emplace_back(
        std::make_shared<StopTime>(std::move(tripId),
                                   std::move(arrivalTime),
                                   std::move(departureTime),
                                   stopId,
                                   stopSequence));
    }
  }
}
