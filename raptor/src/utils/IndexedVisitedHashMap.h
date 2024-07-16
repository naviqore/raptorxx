//
// Created by MichaelBrunner on 13/07/2024.
//

#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>


namespace raptor::utils {

  class IndexedVisitedHashMap
  {
  public:
    explicit IndexedVisitedHashMap(int size);

    void clear();

    void insert(int element, int addElement);

    [[nodiscard]] int getAdditionalElement(int element) const;

    void setAdditionalElement(int element, int addElement);

    [[nodiscard]] std::vector<std::pair<int, int>> getElements() const;

  private:
    std::vector<bool> visited{};
    std::unordered_map<int, int> elements{};
  };

} // utils
// raptor
