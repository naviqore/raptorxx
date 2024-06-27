//
// Created by MichaelBrunner on 31/05/2024.
//

#ifndef AGENCY_H
#define AGENCY_H


#include <string>
#include <cstring>
#include <stdexcept>

namespace schedule::gtfs {
  struct Agency
  {
    Agency(std::string&& aAgencyId, std::string&& aName, std::string&& aTimezone)
      : agencyId(std::move(aAgencyId))
      , name(std::move(aName))
      , timezone(std::move(aTimezone)) {
      if (agencyId.empty()
          || name.empty()
          || timezone.empty())
      {
        throw std::invalid_argument("Mandatory agency fields must not be empty");
      }
    }

    std::string agencyId;
    std::string name;
    std::string timezone;
  };

} // gtfs

#endif //AGENCY_H
