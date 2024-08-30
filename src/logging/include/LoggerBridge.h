//
// Created by MichaelBrunner on 12/06/2024.
//

#pragma once

#include <string>
#include <logging_export.h>

class LOGGER_API LoggerBridge
{
public:
  enum Level
  {
    DEBUG,
    INFO,
    WARN,
    ERR // ERROR is a reserved keyword by windows.h
  };
  virtual ~LoggerBridge() = default;
  virtual void info(const std::string& message) = 0;
  virtual void debug(const std::string& message) = 0;
  virtual void warn(const std::string& message) = 0;
  virtual void error(const std::string& message) = 0;
  virtual void setLevel(Level level) = 0;
  [[nodiscard]] virtual Level getLevel() const = 0;
};
