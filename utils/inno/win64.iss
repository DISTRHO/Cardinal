#include "version.iss"

[Setup]
ArchitecturesInstallIn64BitMode=x64
AppName=Cardinal
AppPublisher=DISTRHO
AppPublisherURL=https://github.com/DISTRHO/Cardinal/
AppSupportURL=https://github.com/DISTRHO/Cardinal/issues/
AppUpdatesURL=https://github.com/DISTRHO/Cardinal/releases/
AppVersion={#VERSION}
DefaultDirName={commonpf64}\Cardinal
DisableDirPage=yes
DisableWelcomePage=no
LicenseFile=..\..\LICENSE
OutputBaseFilename=Cardinal-win64-{#VERSION}-installer
OutputDir=.
UsePreviousAppDir=no

[Types]
Name: "normal"; Description: "Full installation";
Name: "custom"; Description: "Custom installation"; Flags: iscustom;

[Components]
Name: resources; Description: "Resources"; Types: normal custom; Flags: fixed;
Name: carla; Description: "Carla/Ildaeil host tools"; Types: normal;
Name: jack; Description: "Standalone (JACK)"; Types: custom;
Name: native; Description: "Standalone (Native)"; Types: custom;
Name: lv2; Description: "LV2 plugin"; Types: normal;
Name: vst2; Description: "VST2 plugin"; Types: normal;
Name: vst3; Description: "VST3 plugin"; Types: normal;

[Files]
#include "resources.iss"
; icon
Source: "..\..\utils\distrho.ico"; DestDir: "{app}"; Components: resources; Flags: ignoreversion;
; carla
Source: "..\..\carla\bin\carla-bridge-*.*"; DestDir: "{commoncf64}\Cardinal\Carla"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\bin\carla-discovery-*.exe"; DestDir: "{commoncf64}\Cardinal\Carla"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\bin\libcarla_utils.dll"; DestDir: "{commoncf64}\Cardinal\Carla"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\libpython3.8.dll"; DestDir: "{commoncf64}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\Qt5*.dll"; DestDir: "{commoncf64}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\resources\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\iconengines\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources\iconengines"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\imageformats\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources\imageformats"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\platforms\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources\platforms"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\styles\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources\styles"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\resources\lib\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources\lib"; Components: carla; Flags: ignoreversion;
Source: "..\..\carla\build\Carla\resources\lib\PyQt5\*.*"; DestDir: "{commoncf64}\Cardinal\Carla\resources\lib\PyQt5"; Components: carla; Flags: ignoreversion;
; jack
Source: "..\..\bin\Cardinal.exe"; DestDir: "{app}"; Components: jack; Flags: ignoreversion;
; native
Source: "..\..\bin\CardinalNative.exe"; DestDir: "{app}"; Components: native; Flags: ignoreversion;
; lv2
Source: "..\..\bin\Cardinal.lv2\*.*"; DestDir: "{commoncf64}\LV2\Cardinal.lv2"; Components: lv2; Flags: ignoreversion;
Source: "..\..\bin\CardinalFX.lv2\*.*"; DestDir: "{commoncf64}\LV2\CardinalFX.lv2"; Components: lv2; Flags: ignoreversion;
Source: "..\..\bin\CardinalSynth.lv2\*.*"; DestDir: "{commoncf64}\LV2\CardinalSynth.lv2"; Components: lv2; Flags: ignoreversion;
; vst2
Source: "..\..\bin\Cardinal.vst\*.*"; DestDir: "{commoncf64}\VST2\Cardinal.vst"; Components: vst2; Flags: ignoreversion;
; vst3
Source: "..\..\bin\Cardinal.vst3\Contents\x86_64-win\Cardinal.vst3"; DestDir: "{commoncf64}\VST3\Cardinal.vst3\Contents\x86_64-win"; Components: vst3; Flags: ignoreversion;
Source: "..\..\bin\CardinalFX.vst3\Contents\x86_64-win\CardinalFX.vst3"; DestDir: "{commoncf64}\VST3\CardinalFX.vst3\Contents\x86_64-win"; Components: vst3; Flags: ignoreversion;
Source: "..\..\bin\CardinalSynth.vst3\Contents\x86_64-win\CardinalSynth.vst3"; DestDir: "{commoncf64}\VST3\CardinalSynth.vst3\Contents\x86_64-win"; Components: vst3; Flags: ignoreversion;

[Icons]
Name: "{commonprograms}\Cardinal (JACK)"; Filename: "{app}\Cardinal.exe"; IconFilename: "{app}\distrho.ico"; WorkingDir: "{app}"; Comment: "Virtual modular synthesizer plugin (JACK variant)"; Components: jack;
Name: "{commonprograms}\Cardinal (Native)"; Filename: "{app}\CardinalNative.exe"; IconFilename: "{app}\distrho.ico"; WorkingDir: "{app}"; Comment: "Virtual modular synthesizer plugin (Native variant)"; Components: native;
