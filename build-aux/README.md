# `build-aux` folder

This folder contains:
- The Flatpak manifest used to build Lampray

## OBS Studio Flatpak Manifest

The manifest is composed of multiple files:
 - The main manifest `com.github.chollingworth.Lampray.yml`
 - The `modules` folder which contains OBS Studio dependencies modules

### Manifest modules

Modules are ordered/dispatched in numbered categories following a short list of rules:
- A module must not depend on another module from the same category, so a module can only depend on modules from lower numbered categories.
- A module without dependencies must be placed in the highest numbered category in use, excluding categories meant for specific types of dependency.

Actual categories:
 - `90-`: Headers-only libraries that are dependencies of only Lampray
 - `50-`: Modules that are dependencies of only Lamprey
 - `40-`: Modules that are dependencies of the `50-` category