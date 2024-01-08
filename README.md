# ![alt text](https://cdn.discordapp.com/attachments/1160692058017763581/1175551711281168456/LMP-64.png?ex=656ba4d8&is=65592fd8&hm=ccaf321556119a09510514835ccadded74ee8bcc02fb48ec5697d5e9defcdd22&) Lampray 

Lampray is a mod manager for gaming on Linux! If you'd like to help improve Lampray, you can:

- [Report an issue](https://github.com/CHollingworth/Lampray/issues/new?assignees=&labels=bug&projects=&template=-game--bug-report.md&title=)
- [Request a feature](https://github.com/CHollingworth/Lampray/issues/new?assignees=&labels=enhancement&projects=&template=feature_request.md&title=)
- [Join the Discord](https://discord.gg/5macMedevy)
- [Contribute](./CONTRIBUTING.md)

If you'd like to learn more about Lampray, see [Lampray Docs](./docs/lampray-docs.md).

## Quick start

> For a full walkthrough, see [Building Lampray](./docs/building-from-source.md) instead.

Download the [latest release](https://github.com/CHollingworth/Lampray/releases) of Lampray, then generate and configure the build files:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S ./ -B ./Build
```

Open the newly created `Build` directory, then build the Lampray executable:

```bash
ninja
```

Launch Lampray by calling the newly created exeutable:

```bash
~/Lampray/Build/Lampray
```

## Supported games

### Currently supported

- Baldur's Gate 3
- Cyberpunk 2077

### Planned support

- Skyrim (SE & AE)
- Fallout 4
- Elder Scrolls: Oblivion
- Elder Scrolls: Morrowind
- The Sims 4

## Current goals

### Bethesda support

- [ ] Loot Lib Integration
- [ ] FoMod Parsing
- [ ] BSA/ESP Archive Metadata collection

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
