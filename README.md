# ![Lampray](logo/LMP-64.png) Lampray

Lampray is a mod manager for gaming on Linux! If you'd like to help improve Lampray, you can:

- [Report an issue](https://github.com/CHollingworth/Lampray/issues/new?assignees=&labels=bug&projects=&template=-game--bug-report.md&title=)
- [Request a feature](https://github.com/CHollingworth/Lampray/issues/new?assignees=&labels=enhancement&projects=&template=feature_request.md&title=)
- [Join the Discord](https://discord.gg/5macMedevy)
- [Contribute](./CONTRIBUTING.md)

If you'd like to learn more about Lampray, see [Lampray Docs](./docs/lampray-docs.md).

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

> **Note:** In most cases, Lampray can find your 7-zip installation. However, if it's located in a non-standard location, you'll need to [manually set the path to 7-Zip](./docs/customizing-lampray.md#setting-the-path-to-7-zip).

## Quick start

> To build Lampray from source, see [Building from source](./docs/building-from-source.md).

Download the [latest release](https://github.com/CHollingworth/Lampray/releases) of Lampray, then move the executable to any location of your choice. For example:

```bash
mv ~/Downloads/Lampray ~
```

Give Lampray the execute permission.

```bash
chmod +x ~/Lampray
```

Launch the Lampray application.

```bash
~/Lampray/Build/Lampray
```

The first time you run Lampray, the following files and directories will be created:

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

Now you're ready to [mod your game](./docs/managing-mods.md).

## Supported games

### Currently supported

> **Note:** At this time, mods using `.rar` files are not supported.

- Baldur's Gate 3
- Cyberpunk 2077

### Planned support

- The Elder Scrolls V: Skyrim (SE and AE)
- Elder Scrolls IV: Oblivion
- Elder Scrolls III: Morrowind
- Fallout 4
- The Sims 4

## Current goals

### Bethesda support

- [ ] Loot Lib Integration
- [ ] FoMod Parsing
- [ ] BSA/ESP Archive Metadata collection

## Become a donor

If you love using Lampray, consider supporting the project by donating to the development team. For more information, see [our page on Ko-fi](https://ko-fi.com/lampray).

## Special thanks

Special thanks to [airtonix](https://github.com/airtonix) and [SnazzyPanda](https://github.com/SnazzyPanda) and the following members of our community:

- alterNERDtive
- Azurion42
- LT-Batman
- pepper-jk
- GraithTiger
- StuffOfSonny
- nick-scott
- Unhall0w3d
- Jinxtaposition
- w0rldbuilder

Your contributions have been invaluable. May this list continue to grow!
