//
// Created by MichaelBrunner on 30/05/2024.
//

#ifndef ITESTINTERFACE_H
#define ITESTINTERFACE_H

namespace raptor::interfaces {

  class ITestInterface
  {
  public:
    virtual ~ITestInterface() = default;
    virtual void testFunction() = 0;
  };
}

#endif //ITESTINTERFACE_H
