

#include "LoggingPool.h"
#include "LoggerBridgeImpl.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

LoggingPool& LoggingPool::getInstance() {
  static LoggingPool instance;
  return instance;
}

std::shared_ptr<LoggerBridge> LoggingPool::getLogger(Target target) {
  std::lock_guard<std::mutex> lock(mutex);
  if (!loggers.contains(target))
  {
    std::shared_ptr<spdlog::logger> spdLogger;
    switch (target)
    {
      case Target::CONSOLE:
        spdLogger = spdlog::create_async<spdlog::sinks::stdout_color_sink_mt>("console");
        break;
      case Target::FILE:
        spdLogger = spdlog::basic_logger_mt<spdlog::async_factory>("file", "logs/file.log");
        break;
    }
    loggers[target] = std::make_shared<LoggerBridgeImpl>(spdLogger);
  }
  return loggers[target];
}
