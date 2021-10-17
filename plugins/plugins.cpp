/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#include <plugin.hpp>

#include "DistrhoUtils.hpp"

#include "AnimatedCircuits/src/plugin.hpp"
#include "AudibleInstruments/src/plugin.hpp"
#include "Befaco/src/plugin.hpp"
#include "Fundamental/src/plugin.hpp"

Plugin* pluginInstance__AnimatedCircuits;
Plugin* pluginInstance__AudibleInstruments;
Plugin* pluginInstance__Befaco;
Plugin* pluginInstance__Fundamental;

namespace rack {
namespace plugin {

struct StaticPluginLoader {
	Plugin* const plugin;
	FILE* file;
	json_t* rootJ;

	StaticPluginLoader(Plugin* const p, const char* const name)
		: plugin(p),
		  file(nullptr),
		  rootJ(nullptr)
	{
		p->path = system::join(CARDINAL_PLUGINS_DIR, name);

		const std::string manifestFilename = system::join(p->path, "plugin.json");

		if ((file = std::fopen(manifestFilename.c_str(), "r")) == nullptr)
		{
			d_stderr2("Manifest file %s does not exist", manifestFilename.c_str());
			return;
		}

		json_error_t error;
		if ((rootJ = json_loadf(file, 0, &error)) == nullptr)
		{
			d_stderr2("JSON parsing error at %s %d:%d %s", manifestFilename.c_str(), error.line, error.column, error.text);
			return;
		}

		// force ABI, we use static plugins so this doesnt matter as long as it builds
		json_t* const version = json_string((APP_VERSION_MAJOR + ".0").c_str());
		json_object_set(rootJ, "version", version);
		json_decref(version);
	}

	~StaticPluginLoader()
	{
		if (rootJ != nullptr)
		{
			plugin->fromJson(rootJ);
			json_decref(rootJ);
			plugins.push_back(plugin);
		}

		if (file != nullptr)
			std::fclose(file);
	}

	bool ok() const noexcept
	{
		return rootJ != nullptr;
	}
};

static void initStatic__AnimatedCircuits()
{
    Plugin* p = new Plugin;
    pluginInstance__AnimatedCircuits = p;

	const StaticPluginLoader spl(p, "AnimatedCircuits");
	if (spl.ok())
	{
		p->addModel(model_AC_Folding);
	}
}

static void initStatic__AudibleInstruments()
{
    Plugin* p = new Plugin;
    pluginInstance__AudibleInstruments = p;

	const StaticPluginLoader spl(p, "AudibleInstruments");
	if (spl.ok())
	{
		p->addModel(modelBraids);
		p->addModel(modelPlaits);
		p->addModel(modelElements);
		p->addModel(modelTides);
		p->addModel(modelTides2);
		p->addModel(modelClouds);
		p->addModel(modelWarps);
		p->addModel(modelRings);
		p->addModel(modelLinks);
		p->addModel(modelKinks);
		p->addModel(modelShades);
		p->addModel(modelBranches);
		p->addModel(modelBlinds);
		p->addModel(modelVeils);
		p->addModel(modelFrames);
		p->addModel(modelMarbles);
		p->addModel(modelStages);
		p->addModel(modelRipples);
		p->addModel(modelShelves);
		p->addModel(modelStreams);
	}
}

static void initStatic__Befaco()
{
    Plugin* p = new Plugin;
    pluginInstance__Befaco = p;

	const StaticPluginLoader spl(p, "Befaco");
	if (spl.ok())
	{
		p->addModel(modelEvenVCO);
		p->addModel(modelRampage);
		p->addModel(modelABC);
		p->addModel(modelSpringReverb);
		p->addModel(modelMixer);
		p->addModel(modelSlewLimiter);
		p->addModel(modelDualAtenuverter);
	}
}

static void initStatic__Fundamental()
{
    Plugin* p = new Plugin;
    pluginInstance__Fundamental = p;

	const StaticPluginLoader spl(p, "Fundamental");
	if (spl.ok())
	{
		p->addModel(modelVCO);
		p->addModel(modelVCO2);
		p->addModel(modelVCF);
		p->addModel(modelVCA_1);
		p->addModel(modelVCA);
		p->addModel(modelLFO);
		p->addModel(modelLFO2);
		p->addModel(modelDelay);
		p->addModel(modelADSR);
		p->addModel(modelVCMixer);
		p->addModel(model_8vert);
		p->addModel(modelUnity);
		p->addModel(modelMutes);
		p->addModel(modelPulses);
		p->addModel(modelScope);
		p->addModel(modelSEQ3);
		p->addModel(modelSequentialSwitch1);
		p->addModel(modelSequentialSwitch2);
		p->addModel(modelOctave);
		p->addModel(modelQuantizer);
		p->addModel(modelSplit);
		p->addModel(modelMerge);
		p->addModel(modelSum);
		p->addModel(modelViz);
		p->addModel(modelMidSide);
		p->addModel(modelNoise);
		p->addModel(modelRandom);
	}
}

void initStaticPlugins()
{
    initStatic__AnimatedCircuits();
    initStatic__AudibleInstruments();
    initStatic__Befaco();
    initStatic__Fundamental();
}

}
}
