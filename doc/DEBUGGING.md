# Building

This document describes a few possible ideas to help debug Cardinal issues.  
It requires a recent build of Carla, that being >= v2.4.2, in order to work correctly.

Cardinal must be built from source with `make DEBUG=true` for any useful information to be available with these steps.

## Plugin scanning

We can use command-line carla-discovery tools together with valgrind to quickly check for memory errors and leaks.  
By default carla-discovery will do 1 audio processing/run block for testing, which is handy for us here.

```
valgrind --leak-check=full --track-origins=yes --suppressions=./dpf/utils/valgrind-dpf.supp \
/usr/lib/carla/carla-discovery-native vst2 ./bin/CardinalFX.vst/CardinalFX.so
```

## Plugin usage

For regular plugin usage we can use carla-bridge tools, set as dummy mode so audio does not need to run in realtime.  
We set dummy=30 in order to only trigger audio processing every 30s,
otherwise the audio thread would take all of valgrind's time and it would appear as if it was halted.

It is recommended to remove all modules from the Rack except for the strictly necessary ones for debug.

```
env CARLA_BRIDGE_DUMMY=1 \
valgrind --leak-check=full --track-origins=yes --suppressions=./dpf/utils/valgrind-dpf.supp \
/usr/lib/carla/carla-bridge-native vst2 ./bin/CardinalFX.vst/CardinalFX.so ""
```
