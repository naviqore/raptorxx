//
// Created by MichaelBrunner on 31/05/2024.
//

#ifndef AGENCY_H
#define AGENCY_H


#include <string>
#include <cstring>
#include <stdexcept>
#include <schedule_export.h>

namespace schedule::gtfs {
  struct SCHEDULE_API Agency {
    Agency(std::string&& aAgencyId, std::string aName, std::string&& aTimezone)
      : agencyId(std::move(aAgencyId))
      , name(std::move(aName))
      , timezone(std::move(aTimezone))
    {
      if (agencyId.empty()
          || name.empty()
          || timezone.empty()) {
        throw std::invalid_argument("Mandatory agency fields must not be empty");
      }
    }

    std::string agencyId;
    std::string name;
    std::string timezone;
  };

  inline auto agencyHash = [](const Agency& agency) {
    return std::hash<std::string>{}(agency.name);
  };

  inline auto agencyEqual = [](const Agency& lhs, const Agency& rhs) {
    return lhs.name == rhs.name;
  };

} // gtfs

#endif //AGENCY_H
