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

#include "Befaco/src/plugin.hpp"
#include "Fundamental/src/plugin.hpp"

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
    initStatic__Befaco();
    initStatic__Fundamental();
}

}
}
