# RaptorXX

RaptorXX is a C++ project that uses vcpkg for managing its dependencies.

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

