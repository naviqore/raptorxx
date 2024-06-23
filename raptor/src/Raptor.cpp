//
// Created by MichaelBrunner on 23/06/2024.
//

#include "Raptor.h"

namespace raptor {

  Raptor::Raptor(schedule::gtfs::RelationManager relationManager)
    : relationManager(std::move(relationManager)) {
  }
} // raptor