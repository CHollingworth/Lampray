# a small docker file to build lamp on ubuntu
# start with ubuntu minimal
FROM ubuntu:latest

ENV CMAKE_VERSION=3.27
ENV CMAKE_BUILD=0

# update the system
RUN apt-get update \
    && apt-get upgrade -y \
    && apt install -y build-essential libtool autoconf unzip wget libssl-dev

# make a randome directory
RUN mkdir /tmp/build \
    && cd /tmp/build \
    && wget https://cmake.org/files/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.$CMAKE_BUILD.tar.gz \
    && tar -xzvf cmake-$CMAKE_VERSION.$CMAKE_BUILD.tar.gz \
    && cd cmake-$CMAKE_VERSION.$CMAKE_BUILD/ \
    && ./bootstrap \
    && make -j$(nproc) \
    && make install 

# test that cmake is installed and version reported matches CMAKE_VERSION
RUN RESULTING_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3); \
    [ "$RESULTING_VERSION" = "$CMAKE_VERSION.$CMAKE_BUILD" ] || exit 1



# Install Dependencies      
#
# Libaries
#     Vulkan
#     GLFW
#     PkgConfig
#     Pugixml-dev
#     libcurl4-openssl-dev

# System Requirements
#     Zenity

# Compiler & Build
#     C++ 17
#     GCC
#     ninja-build
#     cmake

# Install Dependencies
RUN apt-get install -y \
    libvulkan-dev \
    libglfw3-dev \
    pkg-config \
    libpugixml-dev \
    libcurl4-openssl-dev \
    zenity \
    g++ \
    gcc \
    ninja-build \
    cmake

# Copy the source code
COPY ./build.sh /

# Set the working directory
WORKDIR /src
VOLUME /build

# make it executable
RUN chmod +x /build.sh

# run the build script
CMD /build.sh