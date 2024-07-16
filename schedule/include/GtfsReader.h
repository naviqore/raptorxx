//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADER_H
#define GTFSREADER_H

#include "utils/DataContainer.h"
#include "DataReader.h"
#include "gtfs/GtfsData.h"
#include "Types.h"
#include <functional>
#include <schedule_export.h>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsReader final : public DataReader<gtfsDataContainer>
  {
  public:
    template<typename ReaderType>
    using GtfsStrategy = std::function<void(ReaderType&)>;

    explicit GtfsReader(std::vector<GtfsStrategy<GtfsReader>>&& strategies);

    void readData() override;

    [[nodiscard]] const DataContainer<GtfsData>& getData() const override;

    DataContainer<GtfsData>& getData() override;


  private:
    std::vector<std::function<void(GtfsReader&)>> strategies;

    DataContainer<GtfsData> data;
  };


} // gtfs

#endif //GTFSREADER_H
