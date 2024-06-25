//
// Created by MichaelBrunner on 23/06/2024.
//

#include "GtfsReaderStrategyFactory.h"

#include "strategies/GtfsAgencyReader.h"


namespace schedule::gtfs {
  void GtfsReaderStrategyFactory::init() {
    strategies[AGENCY] = ::gtfs::GtfsAgencyReader("agency.txt");
  }
  GtfsReaderStrategyFactory::GtfsReaderStrategyFactory() {
    init();
  }
  std::function<void(::gtfs::GtfsReader&)>& GtfsReaderStrategyFactory::getStrategy(const Type aType) {
    return strategies[aType];
  }
} // gtfs
  // schedule