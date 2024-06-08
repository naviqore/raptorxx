//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADER_H
#define GTFSREADER_H

#include "DataReader.h"
#include "GtfsData.h"

#include <functional>

#include <gtfs_export.h>


namespace gtfs {

  class GTFS_API GtfsReader final : public DataReader<GtfsData>
  {
  public:
    template<typename ReaderType>
    using GtfsStrategy = std::function<void(ReaderType&)>;
    // using GtfsStrategy = std::function<void(GtfsReader& /*, ...*/)>; std::function<void(GtfsReader&)>

    explicit GtfsReader(std::vector<GtfsStrategy<GtfsReader>>&& strategies);

    void readData() override;

  private:
    std::vector<std::function<void(GtfsReader&)>> strategies;
  };


} // gtfs

#endif //GTFSREADER_H
