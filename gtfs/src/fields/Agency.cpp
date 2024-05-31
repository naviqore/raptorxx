//
// Created by MichaelBrunner on 31/05/2024.
//

#include "Agency.h"

#include <utility>

namespace gtfs {
  Agency::Agency(std::string&& aAgencyId, std::string&& aName, std::string&& aTimezone)
    : agencyId(std::move(aAgencyId))
    , name(std::move(aName))
    , timezone(std::move(aTimezone)) {
  }
} // gtfs
