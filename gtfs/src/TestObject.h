//
// Created by MichaelBrunner on 27/05/2024.
//

#ifndef TEST_H
#define TEST_H



class _declspec(dllexport) TestObject
{
public:
  void testFunction();
};

_declspec(dllexport) void testFunction();



#endif //TEST_H
