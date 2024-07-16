//
// Created by MichaelBrunner on 13/07/2024.
//

#include "IndexedVisitedSet.h"
#include <algorithm>

namespace raptor::utils {

  IndexedVisitedSet::IndexedVisitedSet(int size)
    : visited(size, false) {}

  bool IndexedVisitedSet::isEmpty() const {
    return elements.empty();
  }

  void IndexedVisitedSet::clear() {
    std::ranges::fill(visited, false);
    elements.clear();
  }

  void IndexedVisitedSet::insert(int const element) {
    if (!visited[element])
    {
      visited[element] = true;
      elements.push_back(element);
    }
  }

  const std::vector<int>& IndexedVisitedSet::getElements() const {
    return elements;
  }

  bool IndexedVisitedSet::contains(int element) const {
    return visited[element];
  }
} // utils
  // raptor