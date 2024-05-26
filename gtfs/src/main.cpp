// raptorxx.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


int main() {
  std::cout << "Hello CMake." << std::endl;

  auto console = spdlog::stdout_color_mt("console");
  spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
  spdlog::warn("This is a warning message {}", 1234);

  return 0;
}
