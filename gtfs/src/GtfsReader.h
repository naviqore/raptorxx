//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADER_H
#define GTFSREADER_H

#include "DataReader.h"
#include "GtfsData.h"
#include "model/Agency.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace gtfs {

  class GtfsReader final : public DataReader
  {

  public:
    enum GTFS_TYPE
    {
      agency,
    };
    using GtfsStrategy = std::function<void(GtfsReader& /*, ...*/)>;

    // explicit GtfsReader(std::string_view filename, GtfsStrategy&& strategy);

    explicit GtfsReader(std::vector<std::function<void(GtfsReader&)>>&& strategies);

    void readData() override;

    [[nodiscard]] const GtfsData& getData() const override;

    GtfsData& getData();


  private:
    std::vector<std::function<void(GtfsReader&)>> strategies;
    GtfsData data;
  };


} // gtfs

#endif //GTFSREADER_H
