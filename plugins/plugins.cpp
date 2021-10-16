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

#include "Fundamental/src/plugin.hpp"

Plugin* pluginInstance__Fundamental;

namespace rack {
namespace plugin {

static void initStatic__Fundamental()
{
	Plugin* p = new Plugin;
    pluginInstance__Fundamental = p;
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
    plugins.push_back(p);
}

void initStaticPlugins()
{
    initStatic__Fundamental();
}

}
}
