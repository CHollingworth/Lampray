# Baldur's Gate 3 mod types

> These are the various mod types you can select when [modding your game](../modding-games.md) for Baldur's Gate 3 (BG3).

## Standard mod

Mods using a single `.pak` file. Typically, these are mods that are dependent on other mods. 

!!! example
    ``` shell title="Deployed from"
    ~/Lamp_Data/Deployment/Baldur's Gate 3/Mods
    ```

    ``` shell title="Deployed to"
    ~/users/steamuser/Appdata/Local/Larian Studios/Baldur's Gate 3/Mods
    ```

## Engine injection 

Mods containing DLL's that need to be loaded with the game via NativeModLoader.

!!! example
    ``` shell title="Deployed from"
    ~/Lamp_Data/Deployment/Baldur's Gate 3/bin/NativeMods
    ```

    ``` shell title="Deployed to"
    ~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/bin/NativeMods
    ```

## Bin overwrite

Mods that directly overwrite files in the `/BG3/bin` directory, such as mods that overwrite an executable or core DLLs. This mod type is rare, so it's unlikely you'll need to use this option.

!!! warning
    Lampray cannot undo This operation.

!!! example
    ``` shell title="Deployed from"
    ~/Lamp_Data/Deployment/Baldur's Gate 3/bin
    ```

    ``` shell title="Deployed to"
    ~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/bin
    ```

## Data overwrite

Mods containing loose files, such as textures or meshes for characters and objects. To verify, check if the mod's `.zip` file contains multiple loose files or a single `.pak` file. If it contains loose files, choose **Data Overwrite**.

!!! warning
    Lampray cannot undo This operation.

!!! example
    ``` shell title="Deployed from"
    ~/Lamp_Data/Deployment/Baldur's Gate 3/Data
    ```

    ``` shell title="Deployed to"
    ~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/Data/Generated/Public/SharedDev/Assets/Characters/_Models/path/to/some/modelfile
    ```

## No JSON mod 

Mods that contain a single `.pak` file, but **do not** depend on other mods, such as the ImprovedUI, ModFixer, and Better Containers mods.

!!! example
    ``` shell title="Deployed from"
    ~/Lamp_Data/Deployment/Baldur's Gate 3/Mods
    ```

    ``` shell title="Deployed to"
    ~/.steam/debian-installation/steamapps/compatdata/1086940/pfx/drive_c/users/steamuser/AppData/Local/Larian Studios/Baldurs Gate 3/Mods
    ```
