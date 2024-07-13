//
// Created by MichaelBrunner on 13/07/2024.
//

#include <LoggerFactory.h>


std::shared_ptr<LoggerBridge> getLogger(Target const target, LoggerName const name) {
  static std::unordered_map<LoggerName, std::string> loggerNames = {
    {LoggerName::GTFS, "GTFS"},
    {LoggerName::SCHEDULE, "SCHEDULE"},
    {LoggerName::RAPTOR, "RAPTOR"},
    {LoggerName::GEOMETRY, "GEOMETRY"}};

  return LoggingPool::getInstance().getLogger(target, loggerNames[name]);
}
