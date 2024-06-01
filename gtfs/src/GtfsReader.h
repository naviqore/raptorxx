//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADER_H
#define GTFSREADER_H

#include "DataReader.h"
#include "GtfsData.h"
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

    [[nodiscard]] const GtfsData& getData() const override;

    GtfsData& getData();


  private:
    std::string filename;
    GtfsStrategy strategy;
    GtfsData data;
  };


} // gtfs

#endif //GTFSREADER_H
