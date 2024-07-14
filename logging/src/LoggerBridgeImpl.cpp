//
// Created by MichaelBrunner on 13/07/2024.
//

#include "LoggerBridgeImpl.h"

LoggerBridgeImpl::LoggerBridgeImpl(std::shared_ptr<spdlog::logger> logger)
  : logger(std::move(logger)) {}

void LoggerBridgeImpl::info(const std::string& message) {
  logger->info(message);
}

void LoggerBridgeImpl::warn(const std::string& message) {
  logger->warn(message);
}

void LoggerBridgeImpl::error(const std::string& message) {
  logger->error(message);
}

void LoggerBridgeImpl::debug(const std::string& message) {
  logger->debug(message);
}

void LoggerBridgeImpl::setLevel(const Level level) {
  spdlog::level::level_enum spdLevel = spdlog::level::info;
  switch (level)
  {
    case INFO:
      spdLevel = spdlog::level::info;
    break;
    case WARN:
      spdLevel = spdlog::level::warn;
    break;
    case ERROR:
      spdLevel = spdlog::level::err;
    break;
    case DEBUG:
      [[fallthrough]];
    default:
      spdLevel = spdlog::level::debug;
  }
  logger->set_level(spdLevel);
}

LoggerBridge::Level LoggerBridgeImpl::getLevel() const {
  switch (const auto spdLevel = logger->level())
  {
    case spdlog::level::info:
      return INFO;
    case spdlog::level::warn:
      return WARN;
    case spdlog::level::err:
      return ERROR;
    case spdlog::level::debug:
      [[fallthrough]];
    default:
      return DEBUG;
  }
}