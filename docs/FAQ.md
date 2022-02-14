# Frequently Asked Questions

This document contains a few frequently asked questions (known as "FAQ") regarding the Cardinal project.

## Why does Cardinal exist?

Many reasons, most of them described on the [README](../README.md#why).  
But basically we want an open-source plugin version of "Rack Pro",
where we are free to change things as we see fit, work on new features and fix bugs.  
This is simply not possible with proprietary software, which is the case of "Rack Pro".

## Where is Fundamental?

There are some artwork license issues that prevent us from using Fundamental exactly as we want.  
We could in theory use it as-is, VCV logo and everything, but it looks out of place with Cardinal's general dark mode theme.  
The artwork license does not allow modifications, and that VCV logo being present on the panels makes Cardinal's authors unease.  
Cardinal is not a VCV product in any way, or endorsed by it. Would be quite bad to give that impression.

Current plan is to redo Fundamental panel graphics in a more liberal license, so it then can be included in Cardinal.  
In the mean time, check [this wiki page](https://github.com/DISTRHO/Cardinal/wiki/Fundamental-replacements)
for a list of module replacements for Fundamental stuff.

PS: Don't forget to contribute back as well! ;)

## Can I install extra modules?

No, Cardinal is intentionally a fully self-contained plugin.  
Whatever is contained in the current build is what you can use.  
The exceptions to this are loading files from within a module (like samples)
and regular plugin hosting (using Carla or Ildaeil modules to LV2, VST2, etc).

Adding new modules to Cardinal is only possible by making them be part of build.  
As such it is only possible to include open-source modules.

## So how do I actually add new modules to Cardinal?

By setting up the build to include them.  
This means forking the project, adding git submodules and changing Makefiles.  
Details on this are available [here](https://github.com/DISTRHO/Cardinal/discussions/28).

Due to the highly technical details to sort out for it, such task might not be possible for you at this point.  
Consider just requesting the inclusion of a specific module instead.

Worth noting that, just because a plugin/module is open-source, it does not mean that it can be included in Cardinal.  
Many modules have very strict license terms on the use of its artwork,
or the code can have a license not compatible with Cardinal.  
As a rule of thumb, you can follow these simple rules:

- If module/plugin project license is `GPL-3.0-only` it is not allowed in Cardinal (we want `GPL-3.0-or-later` compatible licenses)
- If the artwork license states "used and distributed with permission", we need to ask for explicit permission to use it
- If the artwork license prohibits derivatives and panels have white or very-bright backgrounds, we need explicit permission for a runtime dark mode

If none of the above apply, it is likely the module is usable as part of Cardinal.  
Also check [this wiki page](https://github.com/DISTRHO/Cardinal/wiki/Possible-modules-to-include)
where we discuss possible modules to include.

## Changes are lost on restart

This is intentional.  
Cardinal is meant to be a self-contained plugin, and as such it does not save any files whatsoever.
This includes user preferences (like list of favourites) or last used project.  
As a plugin, the state will be saved together with the host/DAW project.

## On BSD/Linux/X11 the menu item "Save As/Export..." does nothing

The save-file dialogs in Cardinal requires a working [xdg-desktop-portal](https://github.com/flatpak/xdg-desktop-portal) DBus implementation from your desktop environment.  
Typically your desktop already provides this, if not consider looking for a package to install with "desktop-portal" in the name.

The open-file dialogs in Cardinal do not have this restriction, with a fallback in case desktop portal is not available.

## Why IRC and not Discord?

Discord terms of service are absolute garbage and have no place on a free open-source project.

Don't take it from us, here are a few articles and discussions about it:

- https://drewdevault.com/2021/12/28/Dont-use-Discord-for-FOSS.html
- https://stallman.org/discord.html
- https://discourse.ardour.org/t/ardour-discord-server/88399/
- https://discuss.haiku-os.org/t/discord-server/6519/

Maybe some Matrix channel could be setup, but it would need to bridge to IRC.  
For now IRC works perfectly for Cardinal's authors, so there is no real reason to change.
