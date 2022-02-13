# Cardinal Modules

This file contains documentation for the DISTRHO Cardinal modules.

## Main modules

### Audio File

This is a simple audio file player plugin.  
You can load files by using right-click menu options.

Transport is rolling as long as the plugin is enabled and a file has been loaded.
You can also sync to host transport, which will play in frame-perfect sync, useful if you need a few loops playing together.

Files are streamed from disk if longer than 30 seconds in length, otherwise loaded entirely on RAM.

Even though this player plugin can loop files, it is not an audio looper.
Audio can loop back to the beginning if enabled, but it does not resample or pitch-shift to fit the host BPM.
The audio files are played back as-is, with resampling only done when needed to match the host sample rate.

### Carla

[Carla](https://kx.studio/Applications:Carla) is a fully-featured modular audio plugin host.  
This is a Cardinal/Rack module of Carla with 2 audio and 8 CV IO.  
Double-click on the panel to show the Carla's own window.

Note it is required for Carla to be installed on your system in order to show its GUI.  
This is currently not supported under Windows.

MIDI input and output are possible with the use of expander modules.

### Ildaeil

[Ildaeil](https://github.com/DISTRHO/Ildaeil) is a mini-plugin host based on [Carla](https://kx.studio/Applications:Carla).  
This is Cardinal/Rack module of Ildaeil with 2 audio IO.

Currently Ildaeil supports internal (from Carla) and LV2 plugins.  
There is no way to automate hosted plugin parameters yet.

MIDI input and output are possible with the use of expander modules.

### Host Audio

For getting audio from and to your Host/DAW.  
Works just like the Rack equivalent, including a DC filter that is enabled by default on the stereo version, disabled on the 8 IO variant.

Unlike Rack, Cardinal does not provide a 16 IO audio module.

### Host CV

For getting CV from and to your Host/DAW.

Note that this module does nothing on Cardinal's FX and Synth variants, as they do not have host-exposed CV IO.

### Host MIDI

For getting regular MIDI from and to your Host/DAW.  
This includes ...

### Host MIDI CC

For getting regular MIDI CCs from and to your Host/DAW.  
This includes ...

### Host MIDI Gate

For getting regular MIDI Notes as Gates from and to your Host/DAW.  
...

### Host MIDI Map

For mapping MIDI CCs from your Host/DAW into module parameters.  
This includes ...

### Host Parameters

### Host Time

For getting time information from your DAW as precise clock triggers

## Extra modules

### ExpanderInputMIDI

### ExpanderOutputMIDI

### MPV

### glBars

### Text Editor
