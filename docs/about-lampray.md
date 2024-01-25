# About Lampray

> Reference information about Lampray including dependencies, directory structure, and more. 

## Dependencies

Lampray requires the following:

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

> **Note:** In most cases, Lampray can find your 7-zip installation. However, if it's located in a non-standard location, you'll need to [manually set the path to 7-Zip](./customizing-lampray.md#setting-the-path-to-7-zip).

## Directory structure 

These are the primary directories you'll use to manage and configure Lampray.

```bash
Lampray
└── Build
    └── Lamp_Data 
        ├── Archive
        ├── Config
        └── Mod_Lists 
```

### `Lamp_Data`

The directory where Lampray stores all the necessary files needed for deployment before you [deploy your mods](managing-mods.md#adding-mods). This directory is fully managed by Lampray, so there's no need to modify this directory or its contents.

### `Archives`

The directory that stores your mods' `.zip` files when you drag and drop your mods' `.zip` files into Lampray. This directory is fully managed by Lampray, so there's no need to modify this directory or its contents.

### `Config`

The directory that stores your Lampray configuration files, such as `conf.mdf` which is used to [customize Lampray](customizing-lampray.md).

### `Mod_Lists`

The directory containing your games' `.mdf` files which lists the installed game, its mods, and the corresponding [mod types](./mod-types/index.md). This directory is fully managed by Lampray, so there's no need to modify this directory or its contents.
