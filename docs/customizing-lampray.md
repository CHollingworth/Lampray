# Customizing Lampray

Learn how to customize Lampray's fonts, transparency, color theme, and more.

## Setting the path to 7-Zip

In most cases, Lampray can find your 7-Zip installation automatically. However, if it's located in a non-standard location, you'll need to manually set the path to `7z.so` in Lampray.

First open `~/Lampray/Config/config.mdf` in your text editor, then find and replace the following line with the path to your `7z.so` utility.

```sql
<bit7zLibaryLocation>/usr/lib/p7zip/7z.so</bit7zLibaryLocation>
```

## Customizing your font

You can customize your font by creating a `Lamp_Font` directory within `Lamp_Data` and adding a `.ttf` file. Your file tree should be similar to the following:

```bash
Lamp_Data
├── Archives
├── Config
├── Deployment
├── Lamp_Font 
│   └── terminus.ttf
└── Mod_Lists
```

## Customizing your color theme

You can customize your color theme by editing `Lamp_Data/Config/Conf.mdf`.

### Pre-made Colour Schemes

#### Campfire (by Invisable Friend)
```sql
Colour_Text=#FBF1C7FF
Colour_WindowBackground=#282828FF
Colour_ToolTipAndTypes=#282828FF
Colour_InputBG=#181818FF
Colour_MenuBar=#1D2021FF
Colour_Button=#D65D0EFF
Colour_Hover=#FE8019FF
Colour_Pressed=#AF3A03FF
Colour_MenuItems=#282828FF
Colour_HeadHover=#504945FF
Colour_HeadPressed=#928374FF
Colour_Separator=#282828FF
Colour_SeparatorHover=#282828FF
Colour_SearchHighlight=#504945FF
Colour_ButtonAlt=#98971AFF
```
#### Light Mode

```sql
<Colour_WindowBackground>efefef-ff</Colour_WindowBackground>
<Colour_MenuBar>dbdbdb-ff</Colour_MenuBar>
<Colour_Text>000000-ff</Colour_Text>
<Colour_Button>4296f9-66</Colour_Button>
<Colour_Hover>4296f9-ff</Colour_Hover>
<Colour_Pressed>0f87f9-ff</Colour_Pressed>
<Colour_InputBG>ffffff-ff</Colour_InputBG>
<Colour_Separator>636363-9e</Colour_Separator>
<Colour_SearchHighlight>4c004c-ff</Colour_SearchHighlight>
<Colour_ToolTipAndTypes>ffffff-f9</Colour_ToolTipAndTypes>
```

#### Warlock Purple

```sql
<Colour_SearchHighlight>a61103-ff</Colour_SearchHighlight>
<Colour_Text>ffffff-ff</Colour_Text>
<Colour_WindowBackground>0d0f12-ff</Colour_WindowBackground>
<Colour_ToolTipAndTypes>141414-ff</Colour_ToolTipAndTypes>
<Colour_InputBG>2a0033-ff</Colour_InputBG>
<Colour_MenuBar>3a196d-ff</Colour_MenuBar>
<Colour_Button>300159-ff</Colour_Button>
<Colour_Hover>6703a6-ff</Colour_Hover>
<Colour_Pressed>260101-ff</Colour_Pressed>
<Colour_MenuItems>5f80a7-4f</Colour_MenuItems>
<Colour_HeadHover>4296f9-cc</Colour_HeadHover>
<Colour_HeadPressed>4296f9-ff</Colour_HeadPressed>
<Colour_Separator>2a0133-ff</Colour_Separator>
<Colour_SeparatorHover>1966bf-c6</Colour_SeparatorHover>
```

#### Discord's Clyde's Ai Generated Scheme

```sql
<Colour_WindowBackground>51216B-ef</Colour_WindowBackground>
<Colour_MenuBar>EFD951-ff</Colour_MenuBar>
<Colour_Text>ffffff-ff</Colour_Text>
<Colour_Button>75EF51-66</Colour_Button>
<Colour_Hover>75EF51-ff</Colour_Hover>
<Colour_Pressed>EF7551-ff</Colour_Pressed>
<Colour_InputBG>5151EF-89</Colour_InputBG>
<Colour_Separator>EF5151-7f</Colour_Separator>
<Colour_SearchHighlight>4c004c-ff</Colour_SearchHighlight>
<Colour_ToolTipAndTypes>000CF2-ef</Colour_ToolTipAndTypes>
```
