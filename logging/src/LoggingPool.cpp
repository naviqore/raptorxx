//
// Created by MichaelBrunner on 27/05/2024.
//

#include "LoggingPool.h"
#include <unordered_map>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

// PIMPL idiom - https://en.cppreference.com/w/cpp/language/pimpl
class LoggingPool::LoggingPoolImpl
{
public:
  std::unordered_map<Target, std::unique_ptr<LoggingPool>> instances;
  std::shared_ptr<spdlog::logger> current;
};

std::unique_ptr<LoggingPool::LoggingPoolImpl> LoggingPool::impl = std::make_unique<LoggingPoolImpl>();

LoggingPool::LoggingPool() = default;

std::unique_ptr<LoggingPool> LoggingPool::createInstance() {
  return std::unique_ptr<LoggingPool>(new LoggingPool());
}

LoggerBridge* LoggingPool::getInstance(const Target aTarget) {
  if (!impl->instances.contains(aTarget))
  {
    impl->instances[aTarget] = createInstance();
    switch (aTarget)
    {
      case Target::CONSOLE:
        impl->instances[aTarget]->impl->current = spdlog::stdout_color_mt("console");
        break;
      case Target::FILE:
        impl->instances[aTarget]->impl->current = spdlog::basic_logger_mt("file", "logs/file.log");
        break;
    }
  }
  return impl->instances[aTarget].get();
}

LoggingPool::~LoggingPool() = default;

void LoggingPool::info(const std::string& message) {
  impl->current->info(message);
}
void LoggingPool::warn(const std::string& message) {
  impl->current->warn(message);
}
void LoggingPool::error(const std::string& message) {
  impl->current->error(message);
}
void LoggingPool::setLevel(Level level) {
  impl->current->set_level(static_cast<spdlog::level::level_enum>(level));
}
LoggerBridge::Level LoggingPool::getLevel() const {
  return static_cast<LoggerBridge::Level>(impl->current->level());
}