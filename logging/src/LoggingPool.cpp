

#include "LoggingPool.h"
#include "LoggerBridgeImpl.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

LoggingPool& LoggingPool::getInstance() {
  static LoggingPool instance;
  return instance;
}

std::shared_ptr<LoggerBridge> LoggingPool::getLogger(const Target target, std::string const& name) {
  std::lock_guard<std::mutex> lock(mutex);
  auto key = std::to_string(static_cast<int>(target)) + name;
  if (!loggers.contains(key))
  {
    std::shared_ptr<spdlog::logger> spdLogger;
    switch (target)
    {
      case Target::CONSOLE:
        // spdLogger = spdlog::create_async<spdlog::sinks::stdout_color_sink_mt>(name);
          spdLogger = spdlog::stdout_color_mt(name);
        break;
      case Target::FILE:
        spdLogger = spdlog::basic_logger_mt<spdlog::async_factory>("file", "logs/file.log");
        break;
    }
    loggers[key] = std::make_shared<LoggerBridgeImpl>(spdLogger);
  }
  return loggers[key];
}
