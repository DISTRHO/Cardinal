# LICENSES

## CODE LICENSE / BINARY

While Cardinal itself is licensed under GPLv3+, some modules/plugins used by it are not.  
And since Cardinal builds the entire Rack and modules as a static library,
the more restrictive of the **code licenses** will apply to the final binary.  

Bellow follows a list of all code licenses used in Cardinal and linked submodules.

| Name                  | License(s)            | Additional notes |
|-----------------------|-----------------------|------------------|
| Carla                 | GPL-2.0-or-later      | Used as plugin host within Cardinal|
| DPF                   | ISC, GPL-2.0-or-later | Used as the plugin framework, VST2 binary GPLv2+ licensed |
| Rack                  | GPL-3.0-or-later      | The actual Rack code, internal dependencies are compatible with GPLv3+ |
| AS                    | MIT                   | |
| Amalgamated Harmonics | BSD-3-Clause          | |
| Animated Circuits     | BSD-3-Clause          | |
| Aria Salvatrice       | GPL-3.0-or-later      | |
| Atelier               | GPL-3.0-or-later      | |
| Audible Instruments   | GPL-3.0-or-later      | |
| Bacon Music           | GPL-3.0-or-later      | |
| Befaco                | GPL-3.0-or-later      | |
| Bidoo                 | GPL-3.0-or-later      | |
| Bogaudio              | GPL-3.0-or-later      | |
| ChowDSP               | GPL-3.0-or-later      | |
| cf                    | BSD-3-Clause          | |
| DrumKit               | CC0-1.0               | |
| E-Series              | GPL-3.0-or-later      | |
| Fehler Fabrik         | GPL-3.0-or-later      | |
| Fundamental           | GPL-3.0-or-later      | |
| Glue the Giant        | GPL-3.0-or-later      | |
| Grande                | GPL-3.0-or-later      | |
| HetrickCV             | CC0-1.0               | |
| Impromptu             | GPL-3.0-or-later      | |
| JW-Modules            | BSD-3-Clause          | |
| MindMeld              | GPL-3.0-or-later      | |
| Mog                   | CC0-1.0               | |
| mscHack               | BSD-3-Clause          | |
| Rackwindows           | MIT                   | |
| repelzen              | GPL-3.0-or-later      | |
| Sonus Modular         | GPL-3.0-or-later      | |
| Valley                | GPL-3.0-or-later      | |
| ZZC                   | GPL-3.0-only          | GPLv3+ change request https://github.com/zezic/ZZC/issues/86 |
| ZetaCarinae           | GPL-3.0-or-later      | |

## ARTWORK / PANEL LICENSES

Bellow follows a list of all licenses related to **artwork and module panels**, sorted by file name.  
Licenses were retrieved from the official project's LICENSE, README or related files.  
When * is used, it is meant as wildcard of all files, with potential exceptions mentioned afterwards.  
When a license is uncertain, ?? is used.

The sentence "used and distributed with permission" refers to the original project.  
Since Cardinal does not modify these files, it is assumed their usage is permitted as well.

Note: The "final" version of Cardinal MUST NOT be released with unclear licenses!  
So all uncertainties need to be resolved ASAP.

### Plugins

Below is a list of artwork licenses from plugins

| Name                                    | License(s)       | Additional notes |
|-----------------------------------------|------------------|------------------|
| AmalgamatedHarmonics/*                  | BSD-3-Clause     | No artwork specific license provided |
| AmalgamatedHarmonics/ComponentLibrary/* | CC-BY-NC-4.0     | |
| AmalgamatedHarmonics/DSEG*.ttf          | OFL-1.1-RFN      | |
| AmalgamatedHarmonics/EurostileBold.ttf  | ??               | https://github.com/jhoar/AmalgamatedHarmonics/issues/48 |
| AmalgamatedHarmonics/Roboto-Light.ttf   | Apache-2.0       | |
| AnimatedCircuits/*                      | BSD-3-Clause     | No artwork specific license provided |
| AriaModules/*                           | CC-BY-SA-4.0     | |
| AriaModules/Arcane/*                    | CC-BY-NC-SA-3.0  | Unused in Cardinal |
| AriaModules/components/*                | WTFPL            | |
| AriaModules/dseg/*                      | OFL-1.1-RFN      | |
| AriaModules/lcd/Fixed_v01/*             | Custom           | See [LICENSE.txt](../plugins/AriaModules/res/lcd/Fixed_v01/LICENSE.txt) |
| AriaModules/lcd/piano/*                 | WTFPL            | |
| AriaModules/signature/*                 | Custom           | Removal required if modifying other files without author's permission |
| AS/*                                    | Custom           | Derivative works may not use the AS logo or panel graphics including custom component graphics (knobs, switches, screws, caps,etc.) |
| Atelier/*                               | Custom           | copyright © Pyer 2020, used and distributed with permission |
| AudibleInstruments/*                    | Custom           | Copyright © Emilie Gillet, used and distributed with permission |
| BaconPlugs/*                            | GPL-3.0-or-later | No artwork specific license provided |
| BaconPlugs/midi/*                       | CC-BY-SA-3.0-DE  | |
| BaconPlugs/midi/beeth/*                 | ??               | Taken from http://www.jsbach.net/ |
| BaconPlugs/1f953.svg                    | CC-BY-4.0        | |
| BaconPlugs/Keypunch029.json             | OFL-1.1          | |
| Befaco/components/*                     | CC-BY-NC-4.0     | |
| Befaco/panels/*                         | Custom           | Copyright © [Befaco](https://www.befaco.org/), used and distributed with permission |
| Bidoo/*                                 | GPL-3.0-or-later | No artwork specific license provided |
| BogaudioModules/*                       | CC-BY-SA-4.0     | |
| Cardinal/*                              | CC0-1.0          | |
| Cardinal/Miku/Miku.png                  | CC-BY-NC-3.0     | https://piapro.net/intl/en_for_creators.html |
| ZetaCarinaeModules/*                    | GPL-3.0-or-later | No artwork specific license provided |

TODO: Everything after Cardinal

### Rack

Below is a list of artwork licenses from Rack

| Name                            | License(s)       |
|---------------------------------|------------------|
| ComponentLibrary/*              | CC-BY-NC-4.0     |
| Core/*                          | CC-BY-NC-ND-4.0  |
| fonts/DejaVuSans.ttf            | Bitstream-Vera   |
| fonts/DSEG*.ttf                 | OFL-1.1-RFN      |
| fonts/Nunito-Bold.ttf           | OFL-1.1-RFN      |
| fonts/ShareTechMono-Regular.ttf | OFL-1.1-RFN      |
