//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef TEST_H
#define TEST_H

#include <gtfs_export.h>
#include  <cstdlib>

class GTFS_API TestObject
{
public:
  void testFunction();
};

// JAVA Foreign Function and Memory API - test function
extern "C" GTFS_API size_t stringLength(const char* str);



#endif //TEST_H
