//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include <chrono>
#include <schedule_export.h>

namespace schedule::utils {

  SCHEDULE_API std::chrono::year_month_day parseDate(const std::string& date_str);

} // utils
// schedule
