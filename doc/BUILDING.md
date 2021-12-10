# Building

> Note that you can likely also get a build directly by logging in to Github and heading to our [CI builds page](https://github.com/DISTRHO/Cardinal/actions/workflows/build.yml), the latest successful build has the files at the bottom of the page.

To build Cardinal locally on Debian-based Linux, you need a few dependencies (taken from the Github CI [build.yml](.github/workflows/build.yml)):

`sudo apt install libgl1-mesa-dev liblo-dev libx11-dev libxcursor-dev libxext-dev libxrandr-dev`

You also need a basic toolchain and `cmake`:

`sudo apt install cmake build-essential`

It's important to clone the repo including all submodules:

`git clone https://github.com/DISTRHO/Cardinal --recursive`

You can simply build using:

`make -j 2`

To quickly test you build setup you can run `make DEBUG=true NOPLUGINS=true -j $(nproc)`

The build will be in `bin/` where you should have vst2, vst3, lv2 and Jack standalone of all 3 compatible [variants](../README.md#plugin-variants) Full, FX and Synth.  
The plugins expect to remain *within* their parent folder. If you move them around make sure to keep this structure.  
You can alternatively use symlinks from the user-specific locations (this is recommended), e.g. for lv2:
`ln -s <path to repo>/bin/Cardinal.lv2 ~/.lv2/Cardinal.lv2`

There are a few build flags to know about, use them as `FLAG=true` or `false`:

* `DEBUG=true` to enable debugging
* `NOPLUGINS=true` to only build the Cardinal Core plugins for audio and Midi I/O and the fancy blank panel
* `WITH_LTO=true` to enable Link Time Optimization, this significantly increases the build time
* `SKIP_STRIPPING=true` to disable stripping the binaries if you don't need a full debug

#### Keeping up to date

Things are evolving quickly in Cardinal! To keep your local copy up to date with the changes, do:  
```
git pull
git submodule update --init --recursive
```

### Packaging/distributing

If you are a package maintainer, use the flag `SYSDEPS=true` to use the system dependencies.  

Currently, the only existing packages we are aware of are in Arch's AUR: [cardinal.lv2-git](https://aur.archlinux.org/packages/cardinal.lv2-git/) and [cardinal-git](https://aur.archlinux.org/packages/cardinal-git/).

Feel free to get in touch via Discussions tab or on [IRC](../README.md#community-chat)