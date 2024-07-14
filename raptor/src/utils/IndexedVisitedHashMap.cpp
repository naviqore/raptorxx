//
// Created by MichaelBrunner on 13/07/2024.
//

#include "IndexedVisitedHashMap.h"


namespace raptor::utils {

  IndexedVisitedHashMap::IndexedVisitedHashMap(const int size)
    : visited(size, false) {}

  void IndexedVisitedHashMap::clear() {
    std::ranges::fill(visited, false);
    elements.clear();
  }
  void IndexedVisitedHashMap::insert(const int element, const int addElement) {
    if (!visited[element])
    {
      visited[element] = true;
      elements[element] = addElement;
    }
  }

  int IndexedVisitedHashMap::getAdditionalElement(const int element) const {
    return elements.at(element);
  }

  void IndexedVisitedHashMap::setAdditionalElement(const int element, const int addElement) {
    if (!visited[element])
    {
      elements[element] = addElement;
    }
  }

  std::vector<std::pair<int, int>> IndexedVisitedHashMap::getElements() const {
    std::vector<std::pair<int, int>> result;
    for (const auto& elem : elements)
    {
      if (visited[elem.first])
      {
        result.emplace_back(elem);
      }
    }
    return result;
  }
} // utils
  // raptor