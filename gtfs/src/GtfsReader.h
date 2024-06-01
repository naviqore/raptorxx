//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADER_H
#define GTFSREADER_H

#include "DataReader.h"
#include "fields/Agency.h"

#include <functional>
#include <memory>
#include <string>

namespace gtfs {

  class GtfsReader final : public DataReader
  {
  public:
    using GtfsStrategy = std::function<void(GtfsReader& /*, ...*/)>;

    explicit GtfsReader(std::string_view filename, GtfsStrategy&& strategy);

    void readData() override;

    [[nodiscard]] std::string const& getFilename() const;

    std::vector<Agency> agencies;

  private:
    std::string filename;
    GtfsStrategy strategy;

  };


} // gtfs

#endif //GTFSREADER_H
