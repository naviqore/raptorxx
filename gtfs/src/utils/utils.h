//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> split_line(const std::string& line, const char delimiter = ',') {
  std::vector<std::string> fields;
  std::istringstream lineStream(line);
  std::string field;
  while (std::getline(lineStream, field, delimiter))
  {
    fields.push_back(field);
  }
  return fields;
}

#endif //UTILS_H
