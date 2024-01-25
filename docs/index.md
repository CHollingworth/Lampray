# Getting started

Lampray is a mod manager for gaming on Linux! Learn how to install and launch Lampray, so you can start [modding your games](./modding-games.md). 

## Setting up Lampray 

### Step 1: Install dependencies 

You can install dependencies manually, or you can use the setup script. To use the setup script, clone the Lampray repository into your home directory, then execute the script:

```
git clone git@github.com:CHollingworth/Lampray.git ~
~/Lampray/setup.sh
``` 

If your setup is successful, you'll see the following output:

```bash
==> 💁 [ASDF] Done ✅
```

#### Dependencies

| Type        | Name                 |
|-------------|----------------------|
| Build Tool  | C++                  |
| Build Tool  | CMake                |
| Build Tool  | GCC                  |
| Build Tool  | ninja-build          |
| Library     | libcurl4-openssl-dev |
| Library     | pkg-config           |
| Library     | SDL2                 |
| System Tool | 7-Zip                |
| System Tool | Zenity               |

### Step 2: Install Lampray

You can use the pre-built executable or build from source to install Lampray.

!!! tip
    If you're new to Linux or the command-line, we recommended using the pre-built executable.

=== "Pre-built executable"

    To install Lampray using the pre-built executable, download the [latest release](https://github.com/CHollingworth/Lampray/releases) of Lampray.

    Move the executable to any location of your choice.
    
    ```bash
    mv ~/Downloads/Lampray ~
    ```
    
    Finally, give Lampray the execute permission.
    
    ```bash
    chmod +x ~/Lampray
    ```

=== "Assisted build"

    To build Lampray using the included build script, then open or clone the Lampray repository.
    
    ```
    git clone git@github.com:CHollingworth/Lampray.git ~
    ```

    Build your Lampray executable using the build script.

    ```bash
    ~/Lampray/build.sh
    ```

=== "Manual build"

    To Lampray manually, open or clone the Lampray repository.

    ```
    git clone git@github.com:CHollingworth/Lampray.git ~
    ``` 

    Generate and configure the build files.
    
    ```bash
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S ./ -B ./Build
    ```
    
    Open the newly created `Build` directory.
    
    ```bash
    cd Build
    ```
    
    Build the Lampray executable.
    
    ```bash
    ninja
    ```

### Step 3: Launch Lampray

To launch Lampray, run the following command:

```bash
~/Lampray/Build/Lampray
```

If they don't already exist, the following files and directories will be created the first time you launch Lampray:

```bash
~
├── imgui.ini
├── Lamp_Data
│   ├── Archives
│   ├── Config
│   ├── Deployment
│   └── Mod_Lists
├── lamp.log
└── Lampray
```

!!! tip
    For more information, see [Directory structure](./directory-structure.md).

## Next steps 

Now that you've installed and launched Lampray, you can start [modding your games](./modding-games.md) and [customizing Lampray](./customizing-lampray.md)!
