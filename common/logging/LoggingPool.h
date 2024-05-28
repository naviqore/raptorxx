//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef LOGGINGPOOL_H
#define LOGGINGPOOL_H

#include <unordered_map>
#include "spdlog/logger.h"
#include <memory>

#define COMMON_EXPORTS // TODO add in cmakelists as compile definition

#ifdef _WIN32
    #ifdef COMMON_EXPORTS
        #define COMMON_API __declspec(dllexport)
    #else
        #define COMMON_API __declspec(dllimport)
    #endif
#else
    #ifdef LIBRARY_EXPORTS
        #define COMMON_API __attribute__((visibility("default")))
    #else
        #define COMMON_API
    #endif
#endif

enum class Target : int
{
  CONSOLE,
  FILE
};

class COMMON_API LoggingPool
{
  LoggingPool() = default;
  static std::unordered_map<Target, std::unique_ptr<LoggingPool>> instances;
  std::shared_ptr<spdlog::logger> current;

public:
  static LoggingPool* getInstance(Target aTarget);

  std::shared_ptr<spdlog::logger> getLogger();
};

/*
*  spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
spdlog::warn("This is a warning message {}", 1234);
*/


#endif //LOGGINGPOOL_H
