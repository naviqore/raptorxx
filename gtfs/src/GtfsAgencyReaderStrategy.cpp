//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReaderStrategy.h"

#include <stdexcept>

namespace gtfs {
  GtfsAgencyReaderStrategy::GtfsAgencyReaderStrategy(std::string_view filename)
    : filename(filename) {
    if (filename.empty())
    {
      throw std::invalid_argument("Filename cannot be empty");
    }
  }
  void GtfsAgencyReaderStrategy::operator()(GtfsReader const& aReader) const {
  }
} // gtfs




/*
*#include <iostream>
#include <fstream>
#include <string_view>
#include <vector>
#include <ranges> // For std::ranges::istream_view

// Define a generic reader function
template <typename T>
std::vector<T> read_from_file(const std::string_view filename) {
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }

    // Read data into a vector
    std::vector<T> data;
    for (const auto& value : std::ranges::istream_view<T>(file)) {
        data.push_back(value);
    }

    return data;
}

int main() {
    // Example usage: read integers from "agency.txt"
    const auto filename = "agency.txt";
    const auto integers = read_from_file<int>(filename);

    std::cout << "Read " << integers.size() << " integers from " << filename << ":\n";
    for (const auto& value : integers) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    // You can adapt this reader to other types (e.g., double, string) as needed.
    // Just replace 'int' with the desired type.

    return 0;
}
 */