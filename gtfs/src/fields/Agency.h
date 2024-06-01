//
// Created by MichaelBrunner on 31/05/2024.
//

#ifndef AGENCY_H
#define AGENCY_H

#include <string>

namespace gtfs {
  struct Agency
  {
   // explicit Agency(std::string&& aAgencyId, std::string&& aName, std::string&& aTimezone);

    std::string agencyId;
    std::string name;
    std::string timezone;
  };
} // gtfs

#endif //AGENCY_H
