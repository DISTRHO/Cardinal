# Frequently Asked Questions

This document contains a few frequently asked auestions (known as "FAQ") regarding the Cardinal project.

# Why does Cardinal exist?

Many reasons, most of them described on the [README](../README.md#why).  
But basically we want an open-source plugin version of "Rack Pro",
where we are free to change things as we see fit, work on new features and fix bugs.  
This is simply not possible with proprietary software, which is the case of "Rack Pro".

# Changes are lost on restart

This is intentional.  
Cardinal is meant to be a self-contained plugin, and as such it does not save any files whatsoever.
This includes user preferences (like list of favourites) or last used project.  
As a plugin, the state will be saved together with the host/DAW project.

# On FreeBSD and Linux the menu item "Save As/Export..." does nothing

The save-file dialogs in Cardinal requires a working [xdg-desktop-portal](https://github.com/flatpak/xdg-desktop-portal) DBus implementation from your desktop environment.  
Typically your desktop already provides this, if not consider looking for a package to install with "desktop-portal" in the name.

The open-file dialogs in Cardinal do not have this restriction, with a fallback in case desktop portal is not available.

