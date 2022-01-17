# LICENSES

## CODE LICENSE / BINARY

While Cardinal itself is licensed under GPLv3+, some modules/plugins used by it are not.  
And since Cardinal builds the entire Rack and modules as a static library,
the more restrictive of the **code licenses** will apply to the final binary.  

Bellow follows a list of all code licenses used in Cardinal and linked submodules.

| Name                    | License(s)            | Additional notes |
|-------------------------|-----------------------|------------------|
| Carla                   | GPL-2.0-or-later      | Used as plugin host within Cardinal |
| DPF                     | ISC, GPL-2.0-or-later | Used as the plugin framework, VST2 binary GPLv2+ licensed |
| Rack                    | GPL-3.0-or-later      | The actual Rack code, internal dependencies are compatible with GPLv3+ |
| 21kHz                   | MIT                   | |
| Amalgamated Harmonics   | BSD-3-Clause          | |
| Animated Circuits       | GPL-3.0-or-later      | |
| Aria Salvatrice         | GPL-3.0-or-later      | |
| AS                      | MIT                   | |
| Atelier                 | GPL-3.0-or-later      | |
| Audible Instruments     | GPL-3.0-or-later      | |
| Bacon Music             | GPL-3.0-or-later      | |
| Befaco                  | GPL-3.0-or-later      | |
| Bidoo                   | GPL-3.0-or-later      | |
| Bogaudio                | GPL-3.0-or-later      | |
| cf                      | BSD-3-Clause          | |
| ChowDSP                 | GPL-3.0-or-later      | |
| DrumKit                 | CC0-1.0               | |
| E-Series                | GPL-3.0-or-later      | |
| ExpertSleepers Encoders | MIT                   | |
| Extratone               | GPL-3.0-or-later      | |
| Fehler Fabrik           | GPL-3.0-or-later      | |
| Fundamental             | GPL-3.0-or-later      | |
| Glue the Giant          | GPL-3.0-or-later      | |
| Grande                  | GPL-3.0-or-later      | |
| HetrickCV               | CC0-1.0               | |
| Impromptu               | GPL-3.0-or-later      | |
| ihtsyn                  | GPL-3.0-or-later      | |
| JW-Modules              | BSD-3-Clause          | |
| LifeFormModular         | MIT                   | |
| Little Utils            | EUPL-1.2              | |
| Lomas Modules           | GPL-3.0-or-later      | |
| Lyrae Modules           | GPL-3.0-or-later      | |
| MindMeld                | GPL-3.0-or-later      | |
| Mog                     | CC0-1.0               | |
| mscHack                 | BSD-3-Clause          | |
| Prism                   | BSD-3-Clause          | |
| Rackwindows             | MIT                   | |
| repelzen                | GPL-3.0-or-later      | |
| Sonus Modular           | GPL-3.0-or-later      | |
| Valley                  | GPL-3.0-or-later      | |
| ZetaCarinae             | GPL-3.0-or-later      | |
| ZZC                     | GPL-3.0-or-later      | |

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
| 21kHz                                   | MIT              | No artwork specific license provided |
| AmalgamatedHarmonics/*                  | BSD-3-Clause     | No artwork specific license provided |
| AmalgamatedHarmonics/ComponentLibrary/* | CC-BY-NC-4.0     | |
| AmalgamatedHarmonics/DSEG*.ttf          | OFL-1.1-RFN      | |
| AmalgamatedHarmonics/EurostileBold.ttf  | ??               | https://github.com/jhoar/AmalgamatedHarmonics/issues/48 |
| AmalgamatedHarmonics/Roboto-Light.ttf   | Apache-2.0       | |
| AnimatedCircuits/*                      | CC-BY-NC-SA-4.0  | |
| AriaModules/*                           | CC-BY-SA-4.0     | |
| AriaModules/Arcane/*                    | CC-BY-NC-SA-3.0  | Unused in Cardinal |
| AriaModules/components/*                | WTFPL            | |
| AriaModules/dseg/*                      | OFL-1.1-RFN      | |
| AriaModules/lcd/Fixed_v01/*             | Custom           | See [LICENSE.txt](../plugins/AriaModules/res/lcd/Fixed_v01/LICENSE.txt) |
| AriaModules/lcd/piano/*                 | WTFPL            | |
| AriaModules/signature/*                 | Custom           | Removal required if modifying other files without author's permission |
| AS/*                                    | Custom           | Copyright 2017, derivative works may not use the AS logo or panel graphics including custom component graphics (knobs, switches, screws, caps,etc.). |
| Atelier/*                               | Custom           | Copyright © Pyer 2020, used and distributed with permission |
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
| ChowDSP                                 | TODO             | |
| DrumKit                                 | TODO             | |
| E-Series                                | TODO             | |
| ExpertSleepers-Encoders/*               | MIT              | No artwork specific license provided |
| Extratone                               | TODO             | |
| Fehler Fabrik                           | TODO      | |
| Fundamental                             | TODO      | |
| Glue the Giant                          | TODO      | |
| Grande                                  | TODO      | |
| HetrickCV                               | TODO      | |
| Impromptu                               | TODO      | |
| ihtsyn/*                                | GPL-3.0-or-later | No artwork specific license provided |
| ihtsyn/LEDCalculator.ttf                | Custom           | Free for personal use |
| JW-Modules                              | TODO             | |
| LifeFormModular                         | TODO             | |
| LittleUtils/*                           | EUPL-1.2         | |
| LittleUtils/fonts/CooperHewitt-*.ttf    | OFL-1.1-RFN      | |
| LittleUtils/fonts/Overpass-*.ttf        | OFL-1.1-RFN      | |
| LittleUtils/fonts/RobotoMono-*.ttf      | Apache-2.0       | |
| LomasModules/*                          | GPL-3.0-or-later | No artwork specific license provided, see [LomasModules#26](https://github.com/LomasModules/LomasModules/issues/26) |
| LomasModules/Fonts/FiraMono-Bold.ttf    | OFL-1.1-RFN      | |
| LyraeModules/*                          | CC-BY-NC-SA-4.0  | |
| MindMeld/*                              | CC-BY-NC-ND-4.0  | |
| MindMeld/fonts/RobotoCondensed-*.ttf    | Apache-2.0       | |
| Mog/*                                   | CC0-1.0          | No artwork specific license provided |
| Mog/components/*                        | CC-BY-NC-4.0     | |
| Mog/Exo2-BoldItalic.ttf                 | OFL-1.1-RFN      | |
| mscHack/*                               | BSD-3-Clause     | No artwork specific license provided, see [mschack#108](https://github.com/mschack/VCV-Rack-Plugins/issues/108) |
| Prism/*                                 | CC-BY-SA-4.0     | |
| Prism/RobotoCondensed-Regular.ttf       | Apache-2.0       | |
| Rackwindows/*                           | MIT              | Artwork uses same license as code, see [rackwindows#15](https://github.com/n0jo/rackwindows/issues/15) |
| repelzen/*                              | CC-BY-SA-4.0     | |
| sonusmodular/*                          | GPL-3.0-or-later | No artwork specific license provided, see [sonusmodular#14](https://gitlab.com/sonusdept/sonusmodular/-/issues/14) |
| ValleyAudio/*                           | GPL-3.0-or-later | No artwork specific license provided, see [ValleyRackFree#73](https://github.com/ValleyAudio/ValleyRackFree/issues/73) |
| ValleyAudio/din1451alt.ttf              | CC-BY-3.0-DE     | |
| ValleyAudio/DSEG14Classic-*.ttf         | OFL-1.1-RFN      | |
| ValleyAudio/ShareTechMono-*.ttf         | OFL-1.1-RFN      | |
| ZetaCarinaeModules/*                    | GPL-3.0-or-later | No artwork specific license provided, see [ZetaCarinaeModules#8](https://github.com/mhampton/ZetaCarinaeModules/issues/8) |
| ZZC/*                                   | ??? | See [ZZC#89](https://github.com/zezic/ZZC/issues/89) and [ZZC#90](https://github.com/zezic/ZZC/issues/90) |

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
