# Building Lampray

> Learn how to build Lampray from source.

## Prerequisites

Use your Linux distribution's recommended package manager to install the following dependencies:

| Type        | Name                 | Version          |
|-------------|----------------------|------------------|
| Library     | SDL2                 | 2.0              |
| Library     | pkg-config           | 0.29.2           |
| Library     | libcurl4-openssl-dev | 7.64.0 or later  |
| System Tool | Zenity               | 1.8.2 or later   |
| Build Tool  | C++                  | C++ 17           |
| Build Tool  | GCC                  | 13.2 or later    |
| Build Tool  | ninja-build          | 1.11.1 or later  |
| Build Tool  | CMake                | 3.26 or later    |

## Building from source

Download the [latest release](https://github.com/CHollingworth/Lampray/releases) of Lampray into your home directory, then open it in your terminal.

```bash
cd ~/Lampray
```

Generate and configure Lampray's build files by running the following command:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S ./ -B ./Build
```

Open the newly created `Build` directory.

```bash
cd Build
```

Finally, build the Lampray executable by running the following command:

```bash
ninja
```

Now you can launch Lampray at anytime by running the following command:

```bash
~/Lampray/Build/Lampray
```

