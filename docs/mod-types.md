# Mod types 

> These are the various mod types you can select when [modding your game](./modding-your-game.md).

## Standard Mods

Mods using a single `.pak` file. Typically this would be mods that are dependent on other mods.

### Example

**Deployed from:**

```bash
~/Lampray/Build/Lamp_Data/Deployment/Baldur's Gate 3/Mods
```

**Deployed to:**

```bash
~/users/steamuser/Appdata/Local/Larian Studios/Baldur's Gate 3/Mods
```

## Engine Injection 

Mods containing DLL's that need to be loaded with the game via NativeModLoader. 

### Example

**Deployed from:**

```bash
~/Lampray/Build/Lamp_Data/Deployment/Baldur's Gate 3/bin/NativeMods
```

**Deployed to:**

```bash
~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/bin/NativeMods
```

## Bin Overwrite

Mods that directly overwrite files in the `/BG3/bin` directory, such as mods that overwrite an executable or core DLLs. (Only a few mods will ever need this.) _(This operation cannot be undone by Lamp)._

### Example

**Deployed from:**

```bash
~/Lampray/Build/Lamp_Data/Deployment/Baldur's Gate 3/bin
```

**Deployed to:**

```bash
~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/bin
```

## Data Overwrite

Mods containing loose files, such as textures or meshes for characters and objects. To verify, check if the mod's `.zip` file contains multiple loose files or a single `.pak` file. If contians loose files, choose **Data Overwrite**. _(This operation cannot be undone by Lamp)._

### Example

**Deployed from:**

```bash
~/Lampray/Build/Lamp_Data/Deployment/Baldur's Gate 3/Data
```

**Deployed to:**

```bash
~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/Data/Generated/Public/SharedDev/Assets/Characters/_Models/path/to/some/modelfile
```

## No Json Mod 

Mods that contain a single `.pak` file, but **do not** depend on other mods, such as the ImprovedUI, ModFixer, and Better Containers mods.

### Example

**Deployed from:**

```bash
~/Lampray/Build/Lamp_Data/Deployment/Baldur's Gate 3/Mods
```

**Deployed to:**

```bash
~/.steam/debian-installation/steamapps/compatdata/1086940/pfx/drive_c/users/steamuser/AppData/Local/Larian Studios/Baldurs Gate 3/Mods
```

