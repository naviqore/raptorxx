//
// Created by MichaelBrunner on 27/05/2024.
//

/*#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif*/


// LoggingPool.h

#pragma once

#include "LoggerBridge.h"
#include <memory>
#include <unordered_map>
#include <mutex>
#include <logging_export.h>

enum class LOGGER_API Target
{
  CONSOLE,
  FILE
};

class LOGGER_API LoggingPool
{
public:
  static LoggingPool& getInstance();

  std::shared_ptr<LoggerBridge> getLogger(Target target, std::string const& name);

  LoggingPool(const LoggingPool&) = delete;

  LoggingPool& operator=(const LoggingPool&) = delete;

private:
  LoggingPool() = default;

  ~LoggingPool() = default;

  std::unordered_map<std::string, std::shared_ptr<LoggerBridge>> loggers;
  std::mutex mutex;
};
