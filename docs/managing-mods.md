# Managing mods

> Learn how to manage your mods using Lampray.

## Prerequisites

Before you start, you'll need to [build Lampray from source](./building-lampray.md).

## Setting the path to Steam

You'll need to set the path your Steam library and game data in Lampray, so your mods are added to the right directories. First, open your terminal and launch Lampray:

```bash
~/Lampray/Build/Lampray
```

At the top of the window, select **Lampray** > **Steam Directory**, then follow the on-screen instructions to set the path to your Steam game's root directory and `AppData` directory.

## Adding mods

To add your mods to Lampray, first download your mods from Nexus Mods.

> **NOTE:** At this time, Lampray only supports `.zip` packages, **not** `.rar` packages.

Next, open your terminal and launch Lampray:

```bash
~/Lampray/Build/Lampray
```
At the top corner, choose the game you'd like to mod.

Drag and drop the `.zip` packages for each mod into Lampray.

Enable each mod, choose their mod type, then set their load order. For more information about each mod type, see [Mod types](about-lampray/mod-types.md). 

Select **Deploy**.

When Lampray is finished deploying your mods, exit Lampray, then launch your game.

## Removing mods

To remove your mods from Lampray, open your terminal and launch Lampray:

```bash
~/Lampray/Build/Lampray
```
At the top corner, choose the game you'd like to remove a mod from.

For each mod you'd like to remove, select **Disable**, then **Remove Mod**.

When you're finished, select **Deploy**.

