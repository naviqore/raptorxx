//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADER_H
#define GTFSREADER_H

#include "utils/DataContainer.h"
#include "DataReader.h"
#include "GtfsData.h"
#include "Types.h"

#include <functional>

#include <schedule_export.h>

namespace gtfs {

  class GTFS_API GtfsReader : public schedule::DataReader<gtfsDataContainer>
  {
  public:
    template<typename ReaderType>
    using GtfsStrategy = std::function<void(ReaderType&)>;

    explicit GtfsReader(std::vector<GtfsStrategy<GtfsReader>>&& strategies);

    void readData() override;

    [[nodiscard]] const schedule::DataContainer<GtfsData>& getData() const override;

    schedule::DataContainer<GtfsData>& getData() override;


  private:
    std::vector<std::function<void(GtfsReader&)>> strategies;

    schedule::DataContainer<GtfsData> data;
  };


} // gtfs

#endif //GTFSREADER_H
