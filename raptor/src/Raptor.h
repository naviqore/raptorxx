//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef RAPTOR_H
#define RAPTOR_H
#include <utility>

#include "gtfs/GtfsData.h"
#include "gtfs/RelationManager.h"

namespace raptor {

  class Raptor
  {
    constexpr auto INFINITY_VALUE = std::numeric_limits<int>::max();
    schedule::gtfs::RelationManager relationManager;

  public:
    explicit Raptor(schedule::gtfs::RelationManager  relationManager);
  };

} // raptor

#endif //RAPTOR_H
