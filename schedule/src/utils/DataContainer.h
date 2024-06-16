//
// Created by MichaelBrunner on 12/06/2024.
//

#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <stdexcept>
#include <utility>

namespace schedule {
  template<typename T>
  class DataContainer final
  {
    T data;

  public:
    DataContainer() = default;
    ~DataContainer() = default;

    [[nodiscard]] T const& get() const {
      return data;
    }

    [[nodiscard]] T& get() {
      return data;
    }

    void set(T&& aData) {
      data = std::move(aData);
    }
  };

}

#endif //DATACONTAINER_H
