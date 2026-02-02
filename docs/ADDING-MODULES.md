# How To Add More Modules

(Parts of this document are adapted from https://github.com/DISTRHO/Cardinal/discussions/28)

## Important Notes

- Only open-source modules can be added, as we need their source.
- Also they must be for v2 and be cross-platform, cross-architecture friendly.
    - The latter meaning at least Linux, macOS and Windows as supported systems, plus both x86/64 and ARM builds successfully.
- If one plugin fails to build for ARM but works on x86, we need to fix that first.
    - It is essential that all added modules work on all the builds produced by CI (GitHub actions right now).
- Knowledge of `git` is a requirement.

## Procedure

### Add the plugin as a submodule
1. Add the new plugin/module in git, as a git submodule:

```bash
$ git submodule add [URL_OF_GIT_SUBMODULE] plugins/[NAME_OF_PLUGIN]
```

For example:

```bash
$ git submodule add https://github.com/cfoulc/cf.git plugins/cf
# maybe needed to adjust the commit of new added repo, so it points to v2
$ pushd plugins/cf
$ git checkout v2
# make sure module's git submodules, if any, are available to this local build
$ git submodule update --init --recursive
# go back to the old path
$ popd
```

### In `plugins/Makefile`

2. Add the module files to build in `plugins/Makefile`
This should be based on the upstream repo Makefile, but ignoring the entry point C++ file (typically `src/plugin.cpp`)
For `cf` module we simply add this, following the already existing definitions on the file:

```Makefile
PLUGIN_FILES += $(filter-out cf/src/plugin.cpp,$(wildcard cf/src/*.cpp))
```

3. Add the build rule for the new files in plugins/Makefile
Near the end of the file there are build rules that define how each one is built. If the upstream Makefile uses custom build definitions (those `-DFOO=BAR` things) we need to manually import them in here.
Again for `cf` module, we add this, just following the others on the same file:

```Makefile
$(BUILD_DIR)/cf/src/%.cpp.o: cf/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CF_CUSTOM),$(call custom_module_names,$(m),cf)) \
		-DpluginInstance=pluginInstance__cf \
		-DFOO=BAR
```

(adjust `cf` name for the relevant one)

## In `plugins/plugins.cpp`

Insert the module entry point in plugins/plugins.cpp, again following the same existing modules already in the file.
Make sure to keep the added stuff in alphabetically order, with "Cardinal" as exception.
This is devided into subsections.

4a. Include the module entry point header, something like:

```cpp
// cf
#include "cf/src/plugin.hpp"
```

4b. Create the plugin instance, something like:

```cpp
Plugin* pluginInstance__cf;
```

4c. Create a static function where the modules are loaded, something like:

```cpp
static void initStatic__cf()
{
    Plugin* const p = new Plugin;
    pluginInstance__cf = p;

    const StaticPluginLoader spl(p, "cf");
    if (spl.ok())
    {
        p->addModel(modelMETRO);
        p->addModel(modelEACH);
        // etc, this should have exactly the same content as the entry point C++ file we are ignoring in the `plugins/Makefile`
    }
}
```

4d. Call into the function defined in the previous step during init, like so:

```cpp
void initStaticPlugins()
{
    initStatic__Cardinal();
#ifndef NOPLUGINS
    initStatic__AmalgamatedHarmonics();
    initStatic__AnimatedCircuits();
    initStatic__AS();
    initStatic__AudibleInstruments();
    initStatic__Befaco();
    initStatic__Bidoo();
    initStatic__BogaudioModules();
    ////////////////////
    initStatic__cf(); // <- this is the new called function
    ////////////////////
    initStatic__ESeries();
    initStatic__Fundamental();
    initStatic__GrandeModular();
    initStatic__ImpromptuModular();
    initStatic__JW();
    initStatic__rackwindows();
    initStatic__ZetaCarinaeModules();
#endif // NOPLUGINS
}
```

And that is the jist of it.

## Extra Setup
There can be additional setup if:

- plugin requires extra libraries, these would go in `dep/Makefile` instead, needs to be done on a case-by-case basis
- plugin has too much noise on its entry point include, see how it was done for `mscHack`:

```cpp
// mscHack
/* NOTE too much noise in original include, do this a different way
// #include "mscHack/src/mscHack.hpp"
*/

/* For each module, add

extern Model* [followed by the model name]

You can get the full list by looking at the module entry point header (the .hpp file). */

extern Model* modelCompressor; 
extern Model* modelSynthDrums;
extern Model* modelSEQ_6x32x16;
// ...
extern Model* modelMaude_221;
```

- modules in the new collection have name conflicts with existing ones, see how it is solved in `AS`, `Bogaudio` and `JW`:

In `plugins/Makefile`:

```Makefile
# AS

PLUGIN_FILES += $(filter-out AS/src/AS.cpp,$(wildcard AS/src/*.cpp))
PLUGIN_FILES += AS/freeverb/revmodel.cpp

# modules/types which are present in other plugins
AS_CUSTOM = ADSR BpmDisplayWidget LabelDisplayWidget LedLight LowFrequencyOscillator SigDisplayWidget VCA WaveShaper YellowRedLight allpass comb revmodel # <- This is added to prevent a "C++ One definition error" from other module collections with the same module or component name (like "VCA")
AS_CUSTOM_PER_FILE = NumberDisplayWidget # <- This is added to prevent a "C++ One definition error" name conflict from modules in the same collection
```

And in `plugins/plugins.cpp`:

```cpp
/* For every module/component that shares the same name with one in another collection use:

#define model[NAME] model[BRAND][NAME]

followed by

#undef model[NAME]
*/
#define modelADSR modelASADSR // Module
#define modelVCA modelASVCA // Module
#define modelWaveShaper modelASWaveShaper // Module
#define LedLight ASLedLight // Component
#define YellowRedLight ASYellowRedLight // Component
#include "AS/src/AS.hpp"
#undef modelADSR
#undef modelVCA
#undef modelWaveShaper
#undef LedLight
#undef YellowRedLight

/* ... */

/* In the static function, "#define" and "#undef" are only needed for module names, not anything else. */
static void initStatic__AS()
{
    Plugin* const p = new Plugin;
    pluginInstance__AS = p;
    const StaticPluginLoader spl(p, "AS");
    if (spl.ok())
    {
#define modelADSR modelASADSR
#define modelVCA modelASVCA
#define modelWaveShaper modelASWaveShaper
        //OSCILLATORS
        p->addModel(modelSineOsc);
        p->addModel(modelSawOsc);
        //TOOLS
        p->addModel(modelADSR);
        p->addModel(modelVCA);
        /* snip */
        p->addModel(modelWaveShaper);
        /* Rest of the module list has been removed for this example */
#undef modelADSR
#undef modelVCA
#undef modelWaveShaper
    }
}
```

## Troubleshooting

### `C++ One definition error` 

The modules in the collection share the same name/component/function/struct as another module in another (or the same) collection. Read the _Extra Setup_ section for more information.

### The module is invisible in the server browser
TODO:

### Compiling a module has the message `fatal error: rack.hpp: No such file or directory`
TODO:
