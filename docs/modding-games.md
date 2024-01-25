# Modding games

> Learn how to add and remove mods on Linux using Lampray.

!!! warning 
    In most cases, Lampray can find your 7-zip installation. However, if it's located in a non-standard location, you'll need to [manually set the path to 7-Zip](./configuration.md#setting-the-path-to-7-zip).

## Step 1: Set the paths

You'll need to set the path to your Steam library and game data in Lampray, so your mods are added to the right directories.

In Lampray, select **Lampray** > **Steam Directory**, then follow the on-screen instructions to set the path to your Steam game's root directory and `AppData` directory.

## Step 2: Manage your mods

You'll need to complete the following steps anytime you want to add or remove mods.

### Adding mods

!!! warning
    At this time, mods using `.rar` files are not supported.

1. Download your mods from a mod repository, such as Nexus Mods.
2. At the top corner, choose the game you'd like to mod.
3. Drag and drop the `.zip` packages for each mod into Lampray.
4. Enable each mod, choose their mod type, then set their load order. For more information about each mod type, see [Mod types](./mod-types/index.md). 
5. Select **Deploy**.
6. When Lampray is finished deploying your mods, exit Lampray, then launch your game.

### Removing mods

1. At the top corner, choose the game you'd like to remove a mod from.
2. For each mod you'd like to remove, select **Disable**, then **Remove Mod**.
3. When you're finished, select **Deploy**.
