
// LoggerBridgeImpl.h
#pragma once

#include "LoggerBridge.h"
#include "spdlog/spdlog.h"

class LoggerBridgeImpl final : public LoggerBridge
{
public:
  explicit LoggerBridgeImpl(std::shared_ptr<spdlog::logger> logger);

  void info(const std::string& message) override;

  void warn(const std::string& message) override;

  void error(const std::string& message) override;

  void debug(const std::string& message) override;

  void setLevel(Level level) override;

  [[nodiscard]] Level getLevel() const override;

private:

  std::shared_ptr<spdlog::logger> logger;
};
