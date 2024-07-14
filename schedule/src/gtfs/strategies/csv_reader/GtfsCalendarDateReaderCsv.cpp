//
// Created by MichaelBrunner on 14/07/2024.
//

#include "GtfsCalendarDateReaderCsv.h"

#include <utility>
#include <csv2/reader.hpp>
#include <ranges>

namespace schedule::gtfs {

  struct TempCalendarDate
  {
    std::string serviceId;
    std::chrono::year_month_day date;
    CalendarDate::ExceptionType exceptionType;
  };


  GtfsCalendarDateReaderCsv::GtfsCalendarDateReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  void GtfsCalendarDateReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader<csv2::delimiter<','>,
                               csv2::quote_character<'"'>,
                               csv2::first_row_is_header<true>,
                               csv2::trim_policy::trim_whitespace>();

    if (false == reader.mmap(filename))
    {
      throw std::runtime_error("can not read file");
    }
    const auto header = reader.header();
    std::vector<std::string> headerItems;
    for (const auto& headerItem : header)
    {
      std::string value;
      headerItem.read_value(value);
      headerItems.push_back(value);
    }

    std::map<size_t, std::string> headerMap;
    std::map<std::string, std::function<void(std::string const&)>> headerTypes;
    for (const auto& [index, value] : std::views::enumerate(headerItems))
    {
      headerMap[index] = value;
    }
  }
}