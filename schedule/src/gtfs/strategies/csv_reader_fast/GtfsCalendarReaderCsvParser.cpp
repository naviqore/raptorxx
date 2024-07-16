//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsCalendarReaderCsvParser.h"

#include "csv_wrapper.h"

namespace schedule::gtfs {

  GtfsCalendarReaderCsvParser::GtfsCalendarReaderCsvParser(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsCalendarReaderCsvParser::GtfsCalendarReaderCsvParser(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsCalendarReaderCsvParser::operator()(GtfsReader& aReader) const {

    auto reader = io::CSVReader<10, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>

    reader.set_header("service_id", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday", "start_date", "end_date");
    std::string serviceId;
    int monday;
    int tuesday;
    int wednesday;
    int thursday;
    int friday;
    int saturday;
    int sunday;
    std::string start_date;
    std::string end_date;
    reader.read_header(io::ignore_no_column, "service_id", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday", "start_date", "end_date");
    while (reader.read_row(serviceId, monday, tuesday, wednesday, thursday, friday, saturday, sunday, start_date, end_date))
    {

      auto temp = serviceId;

      Calendar::WeekdayServiceHashMap weekdayService = {
        {std::chrono::Monday, monday},
        {std::chrono::Tuesday, tuesday},
        {std::chrono::Wednesday, wednesday},
        {std::chrono::Thursday, thursday},
        {std::chrono::Friday, friday},
        {std::chrono::Saturday, saturday},
        {std::chrono::Sunday, sunday}};

      aReader.getData().get().calendars.try_emplace(
        temp,
        Calendar{std::move(serviceId),
                 std::move(weekdayService),
                 std::move(start_date),
                 std::move(end_date)});
    }
  }
  // gtfs
} // schedule