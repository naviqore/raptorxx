//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReaderStrategy.h"

#include "fields/Agency.h"

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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


int main() {
  // Open the input file
  std::ifstream infile("tfs_agency.txt");
  if (!infile.is_open()) {
    std::cerr << "Error opening file." << std::endl;
    return 1;
  }

  // Create a vector to store agency data
  std::vector<gtfs::Agency> agencies;

  // Read data from the file
  std::string line;
  while (std::getline(infile, line)) {
    gtfs::Agency agency;
    std::size_t comma1 = line.find(',');
    std::size_t comma2 = line.find(',', comma1 + 1);

    if (comma1 != std::string::npos && comma2 != std::string::npos) {
      agency.name = line.substr(0, comma1);
      agency.location = line.substr(comma1 + 1, comma2 - comma1 - 1);
      agency.employees = std::stoi(line.substr(comma2 + 1));
      agencies.push_back(agency);
    }
  }

  // Close the file
  infile.close();

  // Print agency data
  for (const auto& ag : agencies) {
    std::cout << "Agency Name: " << ag.name << std::endl;
    std::cout << "Location: " << ag.location << std::endl;
    std::cout << "Employees: " << ag.employees << std::endl;
    std::cout << "-------------------------" << std::endl;
  }

  return 0;
}*/


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