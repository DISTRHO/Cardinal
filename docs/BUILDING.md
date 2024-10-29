# Building

This document describes how to build Cardinal from source,
if you rather use Cardinal pre-built binaries please check [this wiki page](https://github.com/DISTRHO/Cardinal/wiki/Install) instead.

Before you begin, make sure you have the needed tools installed to build code, such as gcc or clang.  
How to install those for your system is a bit outside the scope of this document.  
It is expected you generally know how to build software by yourself.

Worth noting that, if cloning Cardinal from git, use of submodules is required.  
So either clone with `--recursive` or use `git submodule update --init --recursive` after cloning.  
If you are building from a release tarball you do not need to care about git.

## Build options

Cardinal uses [GNU Make](https://www.gnu.org/software/make/) as build system.  
So you just got to run `make` within the Cardinal main directory in order to build.

There are a few useful options you can pass as arguments when building though.  
Use them as `make SOMEOPTION=SOMEVALUE` syntax. You can specify as many options as you want.

Developer related options:

* `DEBUG=true` build non-stripped debug binaries (terrible performance, only useful for developers)
* `NOSIMD=true` build without SIMD (not recommended, only useful for developers)

Packaging related options:

* `DESTDIR=/path` typical extra install target path (if you are used to packaging, this does what you expect)
* `PREFIX=/usr` prefix used for installation (note that it **must** be set during build time as well)
* `NOOPT=true` do not automatically set well-known optimization flags
* `SKIP_STRIPPING=true` do not automatically strip the binaries
* `SYSDEPS=true` use jansson, libarchive, samplerate and speexdsp system libraries, instead of vendored
* `WITH_LTO=true` enable Link-Time-Optimization, which has performance benefits but significantly increases the build time

Advanced options:

* `HEADLESS=true` build headless version (without gui), useful for embed systems
* `STATIC_BUILD=true` skip building Cardinal core plugins that use local resources (e.g. audio file and plugin host)

The commonly used build environment flags such as `CC`, `CXX`, `CFLAGS`, etc are respected and used.

## FreeBSD

The use of vendored libraries doesn't work on FreeBSD, as such the `SYSDEPS=true` build option is automatically set.  
This means some dependencies that are optional in other systems are required under FreeBSD.

The use of `gmake` instead of `make` is also required.

Dependencies for using system libraries:

```
# common
sudo pkg install -A cmake dbus fftw libglvnd liblo libsndfile libX11 libXcursor libXext libXrandr python3
# system libraries
sudo pkg install -A libarchive libsamplerate jansson speexdsp
```

## Linux

There are a few differences between Linux distributions, this document covers the most common ones.  
Adjust as needed if your distribution is not based on one of these.

### ArchLinux

Dependencies for using system libraries, that is, with `SYSDEPS=true`:

```
# common
sudo pacman -S cmake dbus file fftw libgl liblo libsndfile libx11 libxcursor libxext libxrandr python3
# system libraries
sudo pacman -S libarchive libsamplerate jansson speexdsp
```

Dependencies for vendored libraries:

```
# common
sudo pacman -S cmake dbus file fftw libgl liblo libsndfile libx11 libxcursor libxext libxrandr python3
# needed by vendored libraries
sudo pacman -S wget
```

### Debian

Dependencies for using system libraries, that is, with `SYSDEPS=true`:

```
# common
sudo apt install cmake libdbus-1-dev libgl1-mesa-dev liblo-dev libfftw3-dev libmagic-dev libsndfile1-dev libx11-dev libxcursor-dev libxext-dev libxrandr-dev python3
# system libraries
sudo apt install libarchive-dev libjansson-dev libsamplerate0-dev libspeexdsp-dev
```

Dependencies for vendored libraries:

```
# common
sudo apt install cmake libdbus-1-dev libgl1-mesa-dev liblo-dev libfftw3-dev libmagic-dev libsndfile1-dev libx11-dev libxcursor-dev libxext-dev libxrandr-dev python3
# needed by vendored libraries
sudo apt install wget
```

### Fedora

Dependencies for using system libraries, that is, with `SYSDEPS=true`:

```
# common
sudo dnf install cmake dbus file fftw mesa-libGL liblo libsndfile libX11 libXcursor libXext libXrandr python3
# system libraries
sudo dnf install libarchive libsamplerate speexdsp
```

Dependencies for vendored libraries:

```
# common
sudo dnf install cmake dbus file fftw mesa-libGL liblo libsndfile libX11 libXcursor libXext libXrandr python3
# needed by vendored libraries
sudo dnf install wget
```

## macOS

Installing Xcode and the "Command-Line utilities" add-on is required.  
Additionally you will need `python3` and `wget` from either Homebrew or MacPorts, whatever you prefer.  
You can also install libsndfile in order to make Cardinal's audio file module work. (Otherwise it will support only mp3 files)

If you want to have universal builds similar to the ones officially published by Cardinal, simply setup the environment like this:

```
export CFLAGS="-DMAC_OS_X_VERSION_MAX_ALLOWED=MAC_OS_X_VERSION_10_12 -mmacosx-version-min=10.12 -arch x86_64 -arch arm64"
export CXXFLAGS="${CFLAGS}"
# make etc..
```

## Web assembly

Cardinal uses emscripten for its web version, see the official instructions on installing emscripten [here](https://emscripten.org/docs/getting_started/downloads.html).  
Once installed, setup the build by importing the emscripten environment and setup the default build tools to point to them, like so:

```
source /path/to/emsdk/emsdk_env.sh
export AR=emar
export CC=emcc
export CXX=em++
export NM=emnm
export RANLIB=emranlib
export STRIP=emstrip
```

Then for the actual build we just need to force graphics rendering to use GLES2 instead of the default "desktop" OpenGL mode, like so:

```
make USE_GLES2=true # add any other relevant options..
```

You can place the generated files on a webserver, or run `emrun bin/CardinalNative.html` for an easy way to test it.

Please note the web build only contains CardinalNative, no other variants will be built.  
This is expected and intentional.

## Windows

Cardinal does not support msvc, using mingw is required.  
It also requires a file-system with support for symbolic links, which Windows cannot do.
For these reasons it is only possible to build Cardinal for Windows from a Linux, macOS or any regular POSIX system.

### Cross-compile

For cross-compilation, first install the relevant mingw packages.  
On Ubuntu these are `binutils-mingw-w64-x86-64 g++-mingw-w64-x86-64 mingw-w64`.  
Then build with `AR`, `CC` and `CXX` pointing to the mingw compiler tools, like so:

```
export AR=x86_64-w64-mingw32-gcc
export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-g++
export EXE_WRAPPER=wine # for running generated windows binaries
export PKG_CONFIG=false # ignore pkg-config from base system
# make etc..
```

# Installing

After a successful build you will find the plugin binaries in the `bin/` directory.  
You can either install them to your system using e.g. `make install PREFIX=/some/prefix` (not recommended for local source builds)
or preferably just create a symbolic link on the respective plugin format folders.

Cardinal plugin binaries expect to remain *within* their parent bundle/folder.  
If you move them around make sure to keep their folder structure intact.

If you are a packager you pretty much already know what to do at this point, otherwise regular users might want to do something like:

```
mkdir -p ~/.lv2 ~/.vst ~/.vst3
ln -s $(pwd)/bin/*.lv2 ~/.lv2/
ln -s $(pwd)/bin/*.vst ~/.vst/
ln -s $(pwd)/bin/*.vst3 ~/.vst3/
```

## macOS

If running macOS, use this instead:

```
mkdir -p ~/Library/Audio/Plug-Ins/LV2 ~/Library/Audio/Plug-Ins/VST ~/Library/Audio/Plug-Ins/VST3
ln -s $(pwd)/bin/*.lv2 ~/Library/Audio/Plug-Ins/LV2/
ln -s $(pwd)/bin/*.vst ~/Library/Audio/Plug-Ins/VST/
ln -s $(pwd)/bin/*.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

Note: Official macOS Cardinal builds install in the system-wide `/Library/Audio/Plug-Ins` location.  
Watch out for conflicts if switching between the two builds.

## Windows

Symbolic links are not supported on Windows, so this approach doesn't work there.  
On Windows you will have to copy or move the plugin bundles.  
If you are building from source, it is expected you know where they should go.

# Keeping up to date

Things are evolving quickly in Cardinal! To keep your local copy up to date with the changes, simply do:

```
git pull
git submodule update --init --recursive
# make etc.. again
```
