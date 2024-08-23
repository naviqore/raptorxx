//
// Created by MichaelBrunner on 14/07/2024.
//

#include "GtfsAgencyReaderCsvParser.h"

#include "LoggerFactory.h"

#include <utility>
#include <algorithm>
#include <ranges>

#include "csv_wrapper.h"



namespace schedule::gtfs {

  GtfsAgencyReaderCsvParser::GtfsAgencyReaderCsvParser(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsAgencyReaderCsvParser::GtfsAgencyReaderCsvParser(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsAgencyReaderCsvParser::operator()(GtfsReader& aReader) const {

    try
    {
      auto reader = io::CSVReader<6, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
      reader.set_header("agency_id", "agency_name", "agency_url", "agency_timezone", "agency_lang", "agency_phone");
      std::string agencyId;
      std::string agencyName;
      std::string agencyUrl;
      std::string agencyTimezone;
      std::string agencyLang;
      std::string agencyPhone;
      reader.read_header(io::ignore_no_column, "agency_id", "agency_name", "agency_url", "agency_timezone", "agency_lang", "agency_phone");
      while (reader.read_row(agencyId, agencyName, agencyUrl, agencyTimezone, agencyLang, agencyPhone))
      {
        std::ignore = agencyUrl;
        std::ignore = agencyLang;
        std::ignore = agencyPhone;

        auto tempName = agencyName;
        aReader.getData().get().agencies.try_emplace(tempName,
                                                     Agency{std::move(agencyId),
                                                            std::move(agencyName),
                                                            std::move(agencyTimezone)});
      }

    } catch (const io::error::too_many_columns& e)
    {
      getLogger(Target::CONSOLE, LoggerName::GTFS)->error(e.what());
    }
  }


}
// gtfs
// schedule