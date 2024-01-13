# Directory structure

> These are the various directories that make up Lampray.

## Overview

These are the primary directories you'll use to manage and configure Lampray.

```bash
Lampray
└── Build
    └── Lamp_Data 
        ├── Archive
        ├── Config
        └── Mod_Lists 
```

## `Lamp_Data`

The directory where Lampray stores all the neccessary files needed for deployment before you [deploy your mods](managing-mods.md#adding-mods). This directory is fully managed by Lampray, so there's no need to modify this directory or its contents.

## `Archives`

The directory that stores your mods' `.zip` files when you drag and drop your mods' `.zip` files into Lampray. This directory is fully managed by Lampray, so there's no need to modify this directory or its contents.

## `Config`

The directory that stores your Lampray configuration files, such as `conf.mdf` which is used to [customize Lampray](customizing-lampray.md).

## `Mod_Lists`

The directory containing your games' `.mdf` files which lists the installed game, its mods, and the corresponding [mod types](mod-types.md). This directory is fully managed by Lampray, so there's no need to modify this directory or its contents.
