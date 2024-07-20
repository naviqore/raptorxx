//
// Created by MichaelBrunner on 20/07/2024.
//

#pragma once

#include "RaptorData.h"


#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>

class OrderedHashMap
{
public:
  // Insert a new element
  void insert(const std::string& key, const std::vector<raptor::StopTime>& value) {
    if (!map_.contains(key))
    {
      list_.push_back(key);
    }
    map_[key] = value;
  }

  // Find an element by key
  const std::vector<raptor::StopTime>* find(const std::string& key) const {
    auto it = map_.find(key);
    if (it != map_.end())
    {
      return &it->second;
    }
    return nullptr;
  }

  // Print the map contents
  void print() const {
    for (const auto& key : list_)
    {
      const auto& value = map_.at(key);
      std::cout << key << ": ";
      for (const auto& stopTime : value)
      {
        std::cout << stopTime.departure << " ";
      }
      std::cout << std::endl;
    }
  }

private:
  std::map<std::string, std::vector<raptor::StopTime>> map_;
  std::list<std::string> list_;
};