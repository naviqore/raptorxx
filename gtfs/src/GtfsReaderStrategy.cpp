//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReaderStrategy.h"

#include <iostream>
#include <tuple>

void gtfs::GtfsReaderStrategy::operator()(GtfsReader const& aReader) const {
  std::ignore = aReader;
  // aReader.readData();
  std::cout << "do something in GtfsReaderStrategy" << std::endl;
}
