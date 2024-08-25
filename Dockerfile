# Stage 1: Build environment
FROM gcc:latest AS build

RUN apt-get update && apt-get install -y cmake git curl zip unzip tar

# vcpkg to install dependencies
RUN git clone https://github.com/microsoft/vcpkg.git /usr/src/vcpkg
WORKDIR /usr/src/vcpkg
RUN ./bootstrap-vcpkg.sh

COPY . /usr/src/myapp/
COPY vcpkg.json /usr/src/myapp

WORKDIR /usr/src/myapp
RUN /usr/src/vcpkg/vcpkg install --triplet x64-linux

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/usr/src/vcpkg/scripts/buildsystems/vcpkg.cmake
RUN cmake --build build

# CMD ["/bin/bash"]

# Stage 2: Test execution environment
FROM build AS runtime

WORKDIR /usr/src/myapp

COPY --from=build /usr/src/myapp/output/bin/release /usr/src/myapp/build
CMD for test_exe in /usr/src/myapp/build/*_tests; do echo "Running $test_exe"; "$test_exe"; done

# $ docker build -t my-gcc-linux-app .
# $ docker run -it --rm --name my-running-app my-gcc-linux-app