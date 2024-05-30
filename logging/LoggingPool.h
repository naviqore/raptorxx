//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef LOGGINGPOOL_H
#define LOGGINGPOOL_H

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

#include <unordered_map>
#include "spdlog/logger.h"
#include <memory>


enum class Target : int
{
  CONSOLE,
  FILE
};

class LOGGER_API LoggingPool
{
public:
  static LoggingPool* getInstance(Target aTarget);
  [[nodiscard]] static std::shared_ptr<spdlog::logger> getLogger();

  ~LoggingPool();
  LoggingPool(const LoggingPool&) = delete;
  LoggingPool& operator=(const LoggingPool&) = delete;

private:
  LoggingPool();

  class LoggingPoolImpl;
  static std::unique_ptr<LoggingPoolImpl> impl;
};

/*
*  spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
spdlog::warn("This is a warning message {}", 1234);
*/


#endif //LOGGINGPOOL_H