//
// Created by MichaelBrunner on 13/07/2024.
//

#pragma once
#include "IndexedVisitedSet.h"


#include <algorithm>
#include <unordered_map>
#include <vector>


namespace raptor::utils {

  class IndexedVisitedHashMap : public IndexedVisitedSet
  {
  public:
    explicit IndexedVisitedHashMap(int size);

    void clear();

    void insert(int routeIndex, int stopIndex);

    [[nodiscard]] int getAdditionalElement(int element) const;

    void setAdditionalElement(int element, int addElement);

    [[nodiscard]] std::vector<std::pair<int, int>> getElements() const;

  private:
    std::unordered_map<int, int> elements{};
  };

} // utils
// raptor
