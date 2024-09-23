# RaptorXX

[![CMake Build Matrix](https://github.com/naviqore/raptorxx/actions/workflows/build-matrix.yml/badge.svg)](https://github.com/naviqore/raptorxx/actions/workflows/build-matrix.yml)

RaptorXX is a C++ project designed to manage and process public transportation data, specifically using the GTFS (
General Transit Feed Specification) format. The project leverages modern C++ standards (C++20/C++23).
The primary functionalities of RaptorXX include:

- **Data Management**: Efficiently handling and processing large datasets of public transportation schedules and routes.
- **Algorithm Implementation**: Implementing the RAPTOR (Round-based Public Transit Routing) algorithm for fast and
  efficient route planning.
- **Modular Design**: Organized into multiple subdirectories for logging, geometry, scheduling, and gtfs to raptor
  structures configuration, ensuring a clean and maintainable codebase.
- **Testing and Benchmarking**: Comprehensive test suites and benchmarks to ensure the correctness and performance of
  the implemented algorithms.

The project is built using CMake, with specific configurations for different compilers and build types. It also includes
scripts for downloading and unzipping necessary data files, ensuring that all required resources are available for the
build process.

## Platform Support

RaptorXX can be compiled for both Windows and Linux platforms. The CMake configuration and vcpkg package manager ensure
that all dependencies and build configurations are handled appropriately for these operating systems.

## Dependencies

This project uses vcpkg, a C++ package manager, to manage its dependencies. Before you can build and use this project,
you need to install the dependencies using vcpkg.

Here are the steps to install the dependencies:

1. First, you need to install vcpkg. You can clone it from the GitHub repository and then bootstrap it using the
   provided script. Open a terminal and run the following commands:

```bash
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh
```

1. Once vcpkg is installed, you can install the dependencies of this project. In the root directory of the project,
   there is a vcpkg.json file that lists the dependencies of the project. You can install all the dependencies with the
   following command:

```bash
./vcpkg/vcpkg install
```

This command should be run in the directory where you installed vcpkg.

1. After installing the dependencies, you need to integrate vcpkg with your user-wide environment. This will make
   vcpkg's
   libraries available to CMake. You can do this with the following command:

```bash
./vcpkg/vcpkg integrate install
 ```

## Building the Project

After installing the dependencies, you can build the project using CMake. Here are the steps:
Create a build directory and navigate into it:

Create a build directory and navigate into it:

```bash
mkdir build
cd build
```

Run CMake to generate the build files:

```bash
cmake ..
```

Build the project:

```bash
cmake --build --config Release
```

