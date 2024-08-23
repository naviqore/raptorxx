FROM gcc:latest

RUN apt-get update && apt-get install -y cmake git curl zip unzip tar

RUN git clone https://github.com/microsoft/vcpkg.git /usr/src/vcpkg
WORKDIR /usr/src/vcpkg
RUN ./bootstrap-vcpkg.sh

COPY . /usr/src/myapp/
COPY vcpkg.json /usr/src/myapp
WORKDIR /usr/src/myapp

RUN /usr/src/vcpkg/vcpkg install --triplet x64-linux

# Configure and build the project
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/usr/src/vcpkg/scripts/buildsystems/vcpkg.cmake
RUN cmake --build build

RUN echo "RUNNING TESTS"

RUN ctest --test-dir build --output-on-failure

# $ docker build -t my-gcc-linux-app .
# $ docker run -it --rm --name my-running-app my-gcc-linux-app