//
// Created by MichaelBrunner on 27/05/2024.
//

#include "TestObject.h"

#include "LoggingPool.h"

#include <iostream>
void TestObject::testFunction() {
  // LoggingPool::getInstance(Target::FILE)->getLogger()->info("Hello from Test {}", 123);
  std::cout << "hello world" << std::endl;
}