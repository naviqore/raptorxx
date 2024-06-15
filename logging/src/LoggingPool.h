//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef LOGGINGPOOL_H
#define LOGGINGPOOL_H

/*#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif*/

#include "LoggerBridge.h"


#include <unordered_map>
#include "spdlog/logger.h"
#include <memory>
#include <logging_export.h>


enum class Target : int
{
  CONSOLE,
  FILE
};

class LOGGER_API LoggingPool : public LoggerBridge
{
public:
  static LoggerBridge* getInstance(Target aTarget);

  ~LoggingPool() override;

  LoggingPool(const LoggingPool&) = delete;

  LoggingPool& operator=(const LoggingPool&) = delete;

  void info(const std::string& message) override;

  void warn(const std::string& message) override;

  void error(const std::string& message) override;

  void setLevel(Level level) override;

  [[nodiscard]] Level getLevel() const override;

private:
  LoggingPool();

  static std::unique_ptr<LoggingPool> createInstance();

  class LoggingPoolImpl;

  static std::unique_ptr<LoggingPoolImpl> impl;
};

/*
*  spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
spdlog::warn("This is a warning message {}", 1234);
*/


#endif //LOGGINGPOOL_H
