Welcome to the Lamp wiki!

This Wiki Contains pages for Usage, Building, and Game-Creation.

Id like to thank @Jinxtaposition for providing a better guide than i could for BG3 and Lamp Basics in #37

### Installation
For installation, download the latest release, and then unpacked Lamp into: 
`~/Games/LampModManager`
But just choose whatever folder suits you best. Download it from Nexus or Github and extract it into that folder. 
Then startup Lamp.

### Lamp Folder Layout

First, here's an overview of what files and folders are what.

Mod Manager Root Folder:

`~/path/to/LampModManager`

The folder you have Lamp installed in.

Mod Manager Settings Folder:

`~/path/to/LampModManager/Lamp_Data/Config`

This folder contains a file conf.mdf which contains the paths to your games, which is managed by the Game Configuration button in Lamp. If you have trouble deploying, ensure these settings are correct.

Mod Manager Mods List Folder:

`~/path/to/LampModManager/Lamp_Data/Mod_Lists`

This folder contains a file BG3.mdf which lists the installed games, mods, and their types. Basically what you see in LAMP's main window. Managed by LAMP, best left alone.

Mod Manager Archives Folder:

`~/path/to/LampModManager/Lamp_Data/Deployment/Archives`

This folder contains the mod archives you drag on to the LAMP UI. Basically storage for your archives.

Mod Staging Folder:

`~/path/to/LampModManager/Lamp_Data`

This contains subfolders where the to-be-deployed files are stored. (Upon deployment they are sent to the target folders.)