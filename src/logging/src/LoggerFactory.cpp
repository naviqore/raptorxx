//
// Created by MichaelBrunner on 13/07/2024.
//

#include "LoggingPool.h"


#include <LoggerFactory.h>
#include <unordered_map>


std::shared_ptr<LoggerBridge> getLogger(Target const target, LoggerName const name)
{
  static std::unordered_map<LoggerName, std::string> loggerNames = {
    {LoggerName::GTFS, "GTFS"},
    {LoggerName::SCHEDULE, "SCHEDULE"},
    {LoggerName::RAPTOR, "RAPTOR"},
    {LoggerName::GEOMETRY, "GEOMETRY"},
    {LoggerName::GTFS_TO_RAPTOR_CONVERTER, "GTFS_TO_RAPTOR_CONVERTER"}};

  return LoggingPool::getInstance().getLogger(target, loggerNames[name]);
}

std::shared_ptr<LoggerBridge> getConsoleLogger(const LoggerName name)
{
  return getLogger(Target::CONSOLE, name);
}
