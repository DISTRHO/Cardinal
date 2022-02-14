# PROJECT OVERVIEW

This document describes how the DISTRHO Cardinal project is structured,
so developers and interested third-parties can have an easier time contributing code and resources.

On the root folder the following directories can be seen;

 * [carla](#carla)
 * [deps](#deps)
 * [docs](#docs)
 * [dpf](#dpf)
 * [include](#include)
 * [lv2export](#lv2export)
 * [patches](#patches)
 * [plugins](#plugins)
 * [src](#src)

Going through one by one in alphebetical order we have...

## carla

This directory contains the source code for Carla, a modular plugin host created by falkTX, the same author of Cardinal, DPF and many other projects.  
Cardinal uses Carla as the base for all internal plugin hosting.  
Being GPLv2+ the code license is compatible with Cardinal's GPLv3+.

## deps

3rd-party libraries build setup.  
No Cardinal specific code is hosted here, only external submodules and a Makefile with steps for fetching extra source code and build it.  
The Makefile overrides Rack's `dep.mk` things for a proper static build, and supporting more platforms.

## docs

Here you find several files (like this one you are reading now) describing the Cardinal project.  
It is intentionally not using something like GitHub Wiki so that rehosting does not lose any information.  
Also allows for offline hosting and reading.

## dpf

This directory contains the source code for DPF, the plugin framework used by Cardinal that handles all the complex parts of plugin format support.  
Implementing new plugin formats will be done here.

## include

This directory contains special header files needed to build the original Rack code as required by Cardinal.  
These headers are included before the official Rack ones, allowing us to override some implementation details.

Additionally a few compatiblity headers are present, helping compile the code for more targets than officially supported in Rack.

## lv2export

An experiment for building individual Rack modules directly as LV2 plugins.  
Only quick&dirty hacks so far, nothing interesting to see here yet.

## patches

Public domain or CC0 licensed Rack patches, suitable for use in Cardinal.  
Must be stored as plain text files (not zstd compressed) so they play nicely with git.

## plugins

Module/Plugin related code and build setup.  
Only Cardinal internal modules are hosted here, everything else uses a git submodule reference.

See https://github.com/DISTRHO/Cardinal/discussions/28 for how to add more modules yourself.

## src

The main code for Cardinal, where the magic happens.  
There are quite a few files here, so let's describe them in detail.

### Cardinal / CardinalFX / CardinalSynth

Directories that contain the supported Cardinal plugin variants.  
Everything is a symlink except `DistrhoPluginInfo.h` (setting plugin info) and `Makefile` (set the unique name).

The source code is the same for all the variants, with compiler macros used to tweak behaviour and IO count.

### custom

Here are files that are originally from Rack but fully reimplemented in Cardinal.  
Some of them are just stubs to define function symbols but without an actual implementation, for example disabling network features.

### extra

A few extra files for having access to a few utilities, code borrowed from Carla, which in turn borrowed it from JUCE.  
The important one is `SharedResourcePointer`, as a way to easily manage a shared class lifecycle.

### MOD

Here are files related to packaging for [MOD Devices](https://moddevices.com/).  
These are not used for regular/common builds, only for MOD, where we add a few extra properties to make Cardinal LV2 integrate better with their system.

### override

Here are files that are very close to the original from Rack but required tweaks for Cardinal.  
Extra care is needed to ensure these are kept in sync with the originals.

### Rack

A git submodule reference to the official Rack source code repository.

### AsyncDialog.{cpp,hpp}

Custom Cardinal code for showing a dialog in async fashion, optionally with a callback for when the user clicks "Ok".

### CardinalCommon.{cpp,hpp}

Common Cardinal code used by a few different files.

### CardinalModuleWidget.cpp

Some more common code and a custom `ModuleWidget::onButton` implementation.

### CardinalPlugin.cpp

The DSP/plugin side of the plugin, and also where the global/shared class lifecycle is managed.  
This file implements the DPF `Plugin` class.

### CardinalUI.cpp

The UI-specific side of the plugin, dealing with e.g. Window events.  
This file implements the DPF `UI` class.

### Makefile

The file describing rules for building Rack's code as a rack.a static library.

### Makefile.cardinal.mk

A makefile imported by each of Cardinal's plugin variants, which will build the actual plugin.  
This same file is used by all variants, changing behaviour based on the plugin variant name.

### PluginContext.hpp

And handy but perhaps somewhat hacky `rack::Context` class extension, so internal modules can have direct access to DAW provided data.  
This also extends the base `Plugin` and `UI` classes from DPF, to provide methods needed for Rack Audio/MIDI drivers.

### template.vcv

The default template patch as used by Cardinal

### WindowParameters.hpp

Defines a few methods for saving and restoring Rack Window state, in order to allow many Cardinal/Rack UIs to be open at once even though Rack `settings` is a global.  
Used by `CardinalUI.cpp` and `override/Window.cpp`.
