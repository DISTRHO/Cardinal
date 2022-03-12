#include "version.iss"

[Setup]
ArchitecturesInstallIn64BitMode=x64
AppName=Cardinal
AppPublisher=DISTRHO
AppPublisherURL=https://github.com/DISTRHO/Cardinal/
AppSupportURL=https://github.com/DISTRHO/Cardinal/issues/
AppUpdatesURL=https://github.com/DISTRHO/Cardinal/releases/
AppVersion={#VERSION}
DefaultDirName={commonpf32}\Cardinal
DisableDirPage=yes
DisableWelcomePage=no
LicenseFile=..\..\LICENSE
OutputBaseFilename=Cardinal-win32-{#VERSION}-installer
OutputDir=.
UsePreviousAppDir=no

[Types]
Name: "full"; Description: "Full installation";
Name: "custom"; Description: "Custom installation"; Flags: iscustom;

[Components]
Name: resources; Description: "Resources"; Types: full custom; Flags: fixed;
Name: carla; Description: "Carla/Ildaeil host tools"; Types: full;
Name: lv2; Description: "LV2 plugin"; Types: full;
Name: vst2; Description: "VST2 plugin"; Types: full;
Name: vst3; Description: "VST3 plugin"; Types: full;

[Files]
#include "resources.iss"
; carla
Source: "..\..\carla\bin\carla-bridge-*.*"; DestDir: "{commoncf32}\Cardinal\Carla"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\bin\carla-discovery-*.exe"; DestDir: "{commoncf32}\Cardinal\Carla"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\bin\libcarla_utils.dll"; DestDir: "{commoncf32}\Cardinal\Carla"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\libpython3.8.dll"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\Qt5*.dll"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\iconengines\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\imageformats\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\platforms\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\resources\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\styles\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\resources\lib\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources\lib"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\resources\lib\PyQt5\*.*"; DestDir: "{commoncf32}\Cardinal\Carla\resources\lib\PyQt5"; Components: carla; Flags: ignoreversion;
; lv2
Source: "..\..\bin\Cardinal.lv2\*.*"; DestDir: "{commoncf32}\LV2\Cardinal.lv2"; Components: lv2; Flags: ignoreversion;
Source: "..\..\bin\CardinalFX.lv2\*.*"; DestDir: "{commoncf32}\LV2\CardinalFX.lv2"; Components: lv2; Flags: ignoreversion;
Source: "..\..\bin\CardinalSynth.lv2\*.*"; DestDir: "{commoncf32}\LV2\CardinalSynth.lv2"; Components: lv2; Flags: ignoreversion;
; vst2
Source: "..\..\bin\Cardinal.vst\*.*"; DestDir: "{commoncf32}\VST2\Cardinal.vst"; Components: vst2; Flags: ignoreversion;
; vst3
Source: "..\..\bin\Cardinal.vst3\Contents\x86-win\Cardinal.vst3"; DestDir: "{commoncf64}\VST3\Cardinal.vst3\Contents\x86-win"; Components: vst3; Flags: ignoreversion;
Source: "..\..\bin\CardinalFX.vst3\Contents\x86-win\CardinalFX.vst3"; DestDir: "{commoncf64}\VST3\CardinalFX.vst3\Contents\x86-win"; Components: vst3; Flags: ignoreversion;
Source: "..\..\bin\CardinalSynth.vst3\Contents\x86-win\CardinalSynth.vst3"; DestDir: "{commoncf64}\VST3\CardinalSynth.vst3\Contents\x86-win"; Components: vst3; Flags: ignoreversion;
