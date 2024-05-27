//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef LOGGINGPOOL_H
#define LOGGINGPOOL_H
#include <unordered_map>
#include "spdlog/logger.h"
#include <memory>

enum class Target : int
{
  CONSOLE,
  FILE
};

class LoggingPool
{
  LoggingPool() = default;
  static std::unordered_map<Target, std::unique_ptr<LoggingPool>> instances;
  std::shared_ptr<spdlog::logger> current;

public:
  static LoggingPool* getInstance(Target aTarget);

  std::shared_ptr<spdlog::logger> getLogger();
};

/*
*  spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
spdlog::warn("This is a warning message {}", 1234);
*/


#endif //LOGGINGPOOL_H
