//
// Created by MichaelBrunner on 27/05/2024.
//

#include "LoggingPool.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/stdout_color_sinks.h>

// LoggingPool.cpp
class LoggingPool::LoggingPoolImpl
{
public:
  std::unordered_map<Target, std::unique_ptr<LoggingPool>> instances;
  std::shared_ptr<spdlog::logger> current;
};

std::unique_ptr<LoggingPool::LoggingPoolImpl> LoggingPool::impl = std::make_unique<LoggingPool::LoggingPoolImpl>();

LoggingPool::LoggingPool()= default;

LoggingPool::~LoggingPool() = default;

LoggingPool* LoggingPool::getInstance(const Target aTarget) {
  return impl->instances[aTarget].get();
}

std::shared_ptr<spdlog::logger> LoggingPool::getLogger() {
  return impl->current;
}


