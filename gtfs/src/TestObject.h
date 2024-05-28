//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef TEST_H
#define TEST_H

// TODO just for testing purposes - move later to a separate file

#ifdef _WIN32
    #ifdef LIBRARY_EXPORTS
        #define GTFS_API __declspec(dllexport)
    #else
        #define GTFS_API __declspec(dllimport)
    #endif
#else
    #ifdef LIBRARY_EXPORTS
        #define GTFS_API __attribute__((visibility("default")))
    #else
        #define GTFS_API
    #endif
#endif


class GTFS_API TestObject
{
public:
  void testFunction();
};

GTFS_API void testFunction();



#endif //TEST_H
