
# ![alt text](https://cdn.discordapp.com/attachments/1160692058017763581/1160692177781919794/LMP-64.png?ex=653595d3&is=652320d3&hm=4febcb9e9ed7f1f31f949a3db68e4b465b23922d44f891e78a788b49bd4cb093&) Lamp 
A Mod Manager for games, made for Linux! As an alternative to Vortex and MO2.

This tool is in its early days please see the contributing section to help add more to it, 
For now i am just one person working on this tool updates will be slow.

### Contributors discussion / Early Peeks
If you want early information about lamp, and how things are progressing join me on my very basic development discord.
[discord](https://discord.gg/5macMedevy)

## List of currently supported games
- Baldur's Gate 3

## List of planned support
- [ ] Skyrim (SE & AE)
- [ ] Fallout 4
- [ ] Oblivion
- [ ] Morrowind
- [ ] The Sims 4

## List of currently suppoted archives
- zip
- 7z
- rar (Not supported on arch)

## Current Goals

### Road to 1.1.0
- [X] Profile Support
- [ ] Persistant Data Paths
- [ ] Game Intigration Guide
- [ ] Independed modsettings creation.
- [X] Easy Modlist Sharing
- [X] Full Lamp::Core Rewrite
- [ ] Wayland Native Builds
- [ ] Impliment OverlayFS as a replacement for pure file duplication
      
### Unessential but nice ideas
- [X] Lamp Specific Colour Scheme

### Bethesda Support
- [ ] Loot Lib Intigration
- [ ] FoMod Parsing
- [ ] BSA/ESP Archvive Metadata collection

## Running Requirements
- Vulkan Support
- Zenity
- glfw
- X11
- p7zip-full & p7zip-rar
- Curl

## Building

This will install asdf and other specific versions of tooling required.

```sh
./setup.sh
./build.sh Debug # or ./build.sh Release
```

Testing cicd: 

```sh
act push
```

For more information See Wiki

## Contributing

I welcome contributions from the community to make this tool even better. Whether you're a developer, designer, or a passionate user, your input is valuable.

See [CONTRIBUTING.md](./CONTRIBUTING.md)

### Issues and Feature Requests

If you encounter any issues, bugs, or have ideas for new features, please open an issue on the Issue Tracker.

Thank you for your contributions and support in making this tool better!

### Credits
Creator: CHollingworth 

Pugixml: zeux

bit7z: rikyoz

### Special Thanks
Id like to take a moment to thank these lovely people for helping me fix issue i wouldnt of known existed without their input:
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

  Thank you all for your time, input, and help. May this list continue to grow! 
