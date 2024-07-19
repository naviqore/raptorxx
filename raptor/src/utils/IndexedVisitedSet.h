//
// Created by MichaelBrunner on 13/07/2024.
//

#pragma once

#include <vector>

namespace raptor::utils {

  class IndexedVisitedSet
  {
  public:
    explicit IndexedVisitedSet(int size);

    [[nodiscard]] bool isEmpty() const;

    void clear();

    void insert(int element);

    [[nodiscard]] const std::vector<int>& getElements() const;

    [[nodiscard]] bool contains(int element) const;

  protected:
    std::vector<bool> visited{};
    std::vector<int> elements{};
  };

} // utils
// raptor
