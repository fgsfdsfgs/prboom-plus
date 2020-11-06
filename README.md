## Building the Vita port
### Requirements
* VitaSDK installed and in PATH;
* libFLAC (run `vdpm flac` to install it if it's not present by default);
* CMake.

The required fork of [vitaGL](https://github.com/fgsfdsfgs/vitaGL/tree/texture_matrix) is
present in this repository. You don't need to install it manually.

### Build instructions
1. Create a build folder:
```mkdir vitabuild && cd vitabuild```
2. On Windows under MSYS2 run:

   ```cmake -G"MSYS Makefiles" -DCMAKE_TOOLCHAIN_FILE="$VITASDK/share/vita.toolchain.cmake" ..```

   On Linux omit the `-G` option.
3. Run `make`. This should produce a VPK in the `vitabuild/src` directory.

## Using the Vita port
### Installation
1. Install VPK.
2. Extract `data.zip` from the latest release to root of memory card (`ux0:` or `uma0:` or whatever, hereafter we'll use `ux0` as an example).
3. Copy any supported IWADs (see below) you have to `ux0:/data/prboom/iwads/`.

### Launcher controls
* LTrigger/RTrigger: select tab
* Up/Down: select option
* Left/Right: adjust option
* Cross: activate option
* Start: save settings and launch the game you have selected in the `Profiles` tab (in single-player mode)
* Circle: save settings and exit launcher / cancel selection when in file select dialog
* Triangle: clear current entry

### Supported games / IWADs
To play a game from this list, copy the corresponding files to `ux0:/data/prboom/iwads/`.

| Game                                   | File(s)        |
|----------------------------------------|----------------|
| Shareware Doom *                       | `doom1.wad`    |
| Doom / Ultimate Doom                   | `doom.wad`     |
| Doom II                                | `doom2.wad`    |
| Final Doom: TNT Evilution              | `tnt.wad`      |
| Final Doom: The Plutonia Experiment    | `plutonia.wad` |
| Chex Quest                             | `chex.wad`, [`chex.deh`](https://www.doomworld.com/idgames/?file=utils/exe_edit/patches/chexdeh.zip) |
| [FreeDoom](https://freedoom.github.io/): Phase 1                      | `freedoom.wad` |
| FreeDoom: Phase 2                      | `freedoom2.wad`|
| FreeDM                                 | `freedm.wad`   |
| Hacx                                   | `hacx.wad`     |

\* included in `data.zip` for the latest Vita release

### Loading PWADs (and other custom game files)
Put all custom files into `ux0:/data/prboom/pwads/`, then use the `Files` tab in the launcher to select any custom content you want.
You can select WAD, LMP, DEH and BEX files there. Files will be loaded in the specified order.

### Recording demos
When `Record demo` is set to `On`, the demo is saved to `ux0:/data/prboom/temp/mydemo.lmp`.

### Notes
If the game closes without producing a crash dump or an error message, a file named `ux0:/data/prboom/error.log` should be generated, which contains error messages.

To join a game by IP, select `Game address`, enter the address, then hit `Connect to address`.
The server is a separate program and for now has to be hosted on a PC.
Grab [the latest PrBoom fork release](https://github.com/coelckers/prboom-plus/releases/latest) and use `prboom-plus_server.exe` to run it.

Netgames will only work correctly if all players have selected the same game and set of custom files (stuff in the `Files` tab).
PWAD order does matter.

You can specify custom command line parameters in a [response file](https://doomwiki.org/wiki/Parameter#.40), then load it using the `Override response file` option.
Don't forget to select the correct profile.

You can only change the video mode in the launcher. Resolutions only have effect in Software modes.

This does not support music packs like Chocolate Doom, but you can just use a PWAD full of MP3s/Oggs or specify music overrides in `ux0:/data/prboom/prboom-config.cfg`.

Controls can be rebound from in-game. Check `Options` -> `Setup` -> `Key Bindings`.

You can add custom profiles to the launcher by editing `ux0:/data/prboom/profiles.cfg`. It also saves your PWAD list for each profile.

PrBoom Plus is Boom compatible with some additional features. This is a fork of Graf Zahl's PrBoom fork, which also adds support for a few more modern features like UMAPINFO.
This means you can run `SIGIL.wad` instead of `SIGIL_COMPAT.wad` and other similar PWADs. However this does not run Heretic, Hexen or Strife.

See also the [original README](prboom2/README).

## Credits
- [these people](prboom2/AUTHORS) for PrBoom Plus itself;
- Vita SDK Team for the Vita SDK;
- rsn8887 and cpasjuste for the SDL2 port;
- Rinnegatamante for vitaGL;
- KINGGOLDrus for some launcher graphics.
