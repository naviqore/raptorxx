//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef TEST_H
#define TEST_H

#include <gtfs_export.h>
#include <cstdio>
#include <string>

class GTFS_API TestObject
{
public:
  void testFunction();
};

GTFS_API void readAgencyTestFunction(std::string const& aFileName);

extern "C" GTFS_API size_t stringLength(const char* str);




#endif //TEST_H
