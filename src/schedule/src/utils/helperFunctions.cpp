//
// Created by MichaelBrunner on 26/07/2024.
//

#include "include/model/helperFunctions.h"


namespace schedule::utils {

  std::chrono::year_month_day parseDate(const std::string& date_str)
  {
    const int year = std::stoi(date_str.substr(0, 4));
    const int month = std::stoi(date_str.substr(4, 2));
    const int day = std::stoi(date_str.substr(6, 2));
    return std::chrono::year{year} / month / day;
  }

} // utils
// schedule