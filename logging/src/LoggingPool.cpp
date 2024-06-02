//
// Created by MichaelBrunner on 27/05/2024.
//

#include "LoggingPool.h"
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

LoggingPool* LoggingPool::getInstance(const Target aTarget) {
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

std::shared_ptr<spdlog::logger> LoggingPool::getLogger() {
  return impl->current;
}
LoggingPool::~LoggingPool() = default;