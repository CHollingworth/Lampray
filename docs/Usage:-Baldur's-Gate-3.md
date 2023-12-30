## Mod Type Definitions
And here's an overview of the different Mod Types you can select in LAMP, and what they mean for deployment. Each Mod Type copies mods to a different part of the game directories, depending on what is required by the mod.

### Standard Mods
Mods with **Standard Mod** selected get deployed from the LAMP Deployment folder:

`~/path/to/LampModManager/Lamp_Data/Deployment/Baldur's Gate 3/Mods`

 to the **BG AppData** folder:

`~/users/steamuser/Appdata/Local/Larian Studios/Baldur's Gate 3/Mods`

This basically goes for EVERY mod that uses nothing but a lone **.pak** file, if it requires something to be added to modsettings this is the one to pick!



### Engine Injection 
Mods get deployed from the LAMP NativeMods folder:

`~/path/to/LampModManager/Lamp_Data/Deployment/Baldur's Gate 3/bin/NativeMods`

to the BG3\bin\NativeMods folder:

`~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/bin/NativeMods`

Use this option for mods that contain DLL's that need to be loaded with the game via NativeModLoader. 

### Bin Overwrite
Mods get deployed from:

`~/path/to/LampModManager/Lamp_Data/Deployment/Baldur's Gate 3/bin`

to:

`~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/bin`

This is useful for mods that need to directly overwrite what's present in the **/BG3/bin** folder, e.g. to replace the executable or one of the other core DLLs. (Only a few mods will need this.) _(!This operation cannot be undone by Lamp) _

### Data Overwrite
Mods get deployed from:
`~/path/to/LampModManager/Lamp_Data/Deployment/Baldur's Gate 3/Data`

to:

`~/.steam/debian-installation/steamapps/common/Baldurs Gate 3/Data/Generated/Public/SharedDev/Assets/Characters/_Models/path/to/some/modelfile`

This option is for mods that contain loose files, for example textures or meshes for characters or objects. You can tell by opening the mod zip file and seeing whether it has loose files in some long paths, or just a **.pak** file. If it's loose files, use the **Data Overwrite** option. _(!This operation cannot be undone by Lamp)_

### No Json Mod 
Mod files get deployed from:

`~/path/to/LampModManager/Lamp_Data/Deployment/Baldur's Gate 3/Mods`
to:

`~/.steam/debian-installation/steamapps/compatdata/1086940/pfx/drive_c/users/steamuser/AppData/Local/Larian Studios/Baldurs Gate 3/Mods`

This if for mods that done require anything to be added to the mod-settings, e.g. ImprovedUI, ModFixer, Better Containers.

## Notes on deployment
- I always do things step-by-step. For example, if I accidentally deployed a mod under the wrong Mod Type, I first Disable it, then hit Deploy button to ensure it is removed; only then do I change the Mod Type to whatever I should be using, and then hit Deploy again. 
- When you download a mod, unzip it first and look at the contents, then compare to the descriptions above to figure out what Mod Type to use.
- Don't forget to SHUT DOWN LAMP before you start to play.