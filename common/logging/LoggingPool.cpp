//
// Created by MichaelBrunner on 27/05/2024.
//

#include "LoggingPool.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/stdout_color_sinks.h>


std::unordered_map<Target, std::unique_ptr<LoggingPool>> LoggingPool::instances;

auto getLoggerBasedOnType(const Target aTarget) {
  switch (aTarget)
  {
    case Target::CONSOLE:
      return spdlog::stdout_color_mt("console");
    case Target::FILE:
      return spdlog::basic_logger_mt("file", "log.txt");
  }
  return spdlog::get("console");
}

LoggingPool* LoggingPool::getInstance(const Target aTarget) {
  if (instances.contains(aTarget))
  {
    return instances[aTarget].get();
  }
  auto pool = std::unique_ptr<LoggingPool>();
  pool->current = getLoggerBasedOnType(aTarget);
  instances[aTarget] = std::move(pool);
  return instances[aTarget].get();
}
std::shared_ptr<spdlog::logger> LoggingPool::getLogger() {
  return current;
}