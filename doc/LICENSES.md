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
| Atelier               | GPL-3.0-only          | GPLv3+ change request https://github.com/Xenakios/Atelier/issues/18 |
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
When * is used, it is meant as wildcard of all files, with potential exceptions mentioned afterwards.  
When a license is uncertain, ?? is used.

Note: The "final" version of Cardinal MUST NOT be released with unclear licenses!  
So all uncertainties need to be resolved ASAP.

| Name                                                | License(s)       | Additional notes |
|-----------------------------------------------------|------------------|------------------|
| plugins/AmalgamatedHarmonics/res/*                  | BSD-3-Clause     | No artwork specific license provided |
| plugins/AmalgamatedHarmonics/res/ComponentLibrary/* |                  | |
| plugins/AmalgamatedHarmonics/res/DSEG*.ttf          | OFL-1.1-RFN      | |
| plugins/AmalgamatedHarmonics/res/EurostileBold.ttf  | ??               | https://docs.microsoft.com/en-us/typography/font-list/eurostile |
| plugins/AmalgamatedHarmonics/res/Roboto-Light.ttf   | Apache-2.0       | |
| plugins/AnimatedCircuits/res/*                      | BSD-3-Clause     | No artwork specific license provided |
| plugins/AriaModules/res/*                           | CC-BY-SA-4.0     | |
| plugins/AriaModules/res/Arcane/*                    | CC-BY-NC-SA-3.0  | Unused in Cardinal |
| plugins/AriaModules/res/components/*                | WTFPL            | |
| plugins/AriaModules/res/dseg/*                      | OFL-1.1-RFN      | |
| ..etc TODO | | |
| plugins/Cardinal/*                                  | CC0-1.0          | |
| plugins/Cardinal/res/Miku/Miku.png                  | CC-BY-NC-3.0     | https://piapro.net/intl/en_for_creators.html |
| plugins/ZetaCarinaeModules/res/*                    | GPL-3.0-or-later | No artwork specific license provided |
| src/Rack/res/ComponentLibrary/*                     | CC-BY-NC-4.0     | |
| src/Rack/res/Core/*                                 | CC-BY-NC-ND-4.0  | |
| src/Rack/res/fonts/DejaVuSans.ttf                   | Bitstream-Vera   | |
| src/Rack/res/fonts/DSEG*.ttf                        | OFL-1.1-RFN      | |
| src/Rack/res/fonts/Nunito-Bold.ttf                  | OFL-1.1-RFN      | |
| src/Rack/res/fonts/ShareTechMono-Regular.ttf        | OFL-1.1-RFN      | |
