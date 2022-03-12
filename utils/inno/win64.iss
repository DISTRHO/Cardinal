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
Name: "full"; Description: "Full installation";
Name: "custom"; Description: "Custom installation"; Flags: iscustom;

[Components]
Name: resources; Description: "Resources"; Types: full custom; Flags: fixed;
Name: lv2; Description: "LV2 plugin"; Types: full;
Name: vst2; Description: "VST2 plugin"; Types: full;
Name: vst3; Description: "VST3 plugin"; Types: full;

[Files]
#include "resources.iss"
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
