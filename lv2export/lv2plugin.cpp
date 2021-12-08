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

#ifndef PLUGIN_INSTANCE
# error PLUGIN_INSTANCE undefined
#endif

#ifndef PLUGIN_MODEL
# error PLUGIN_MODEL undefined
#endif

#ifndef PLUGIN_URI
# error PLUGIN_URI undefined
#endif

#define PRIVATE
#include <common.hpp>
#include <engine/Engine.hpp>

#undef PRIVATE
#include <rack.hpp>

#include "src/lv2/buf-size.h"
#include "src/lv2/options.h"

#include "DistrhoUtils.hpp"

using namespace rack;

extern Model* PLUGIN_MODEL;
extern Plugin* PLUGIN_INSTANCE;

namespace rack {
namespace engine {

struct Engine::Internal {
    float sampleRate;
};

Engine::Engine()
{
    internal = new Internal;
}

Engine::~Engine()
{
    delete internal;
}

float Engine::getSampleRate()
{
    return internal->sampleRate;
}

}

namespace plugin {

void Plugin::addModel(Model* model)
{
	// Check that the model is not added to a plugin already
    DISTRHO_SAFE_ASSERT_RETURN(model != nullptr,);
	DISTRHO_SAFE_ASSERT_RETURN(model->plugin == nullptr,);
	model->plugin = this;
	models.push_back(model);
}
Model* modelFromJson(json_t* moduleJ) {
    return nullptr;
}
std::vector<Plugin*> plugins;

} // namespace plugin
} // namespace rack

struct PluginLv2 {
    Context* context;
    Plugin* plugin;
    engine::Module* module;
    float sampleRate;
    int frameCount = 0;
    int numInputs, numOutputs, numParams, numLights;
    void** ports;

    PluginLv2(double sr)
    {
        // FIXME shared instance for these 2
        context = new Context;
        context->engine = new Engine;
        context->engine->internal->sampleRate = sr;
        contextSet(context);
        plugin = new Plugin;
        PLUGIN_INSTANCE = plugin;

        sampleRate = sr;
        plugin->addModel(PLUGIN_MODEL);
        module = PLUGIN_MODEL->createModule();

        numInputs = module->getNumInputs();
        numOutputs = module->getNumOutputs();
        numParams = module->getNumParams();
        numLights = module->getNumLights();
        ports = new void*[numInputs+numOutputs+numParams+numLights];

        // FIXME for CV ports we need to detect if something is connected
        for (int i=numInputs; --i >=0;)
            module->inputs[i].channels = 1;
        for (int i=numOutputs; --i >=0;)
            module->outputs[i].channels = 1;

        d_stdout("Loaded %s :: %i inputs, %i outputs, %i params and %i lights",
                 PLUGIN_URI, numInputs, numOutputs, numParams, numLights);
    }

    PluginLv2()
    {
        contextSet(context);

        delete[] ports;
        delete module;

        // FIXME shared instance for this
        delete plugin;
        delete context;
    }

    void lv2_connect_port(const uint32_t port, void* const dataLocation)
    {
        ports[port] = dataLocation;
    }

    void lv2_activate()
    {
        contextSet(context);
        module->onReset();
    }

    void lv2_run(const uint32_t sampleCount)
    {
        if (sampleCount == 0)
            return;

        contextSet(context);

        Module::ProcessArgs args = {
            sampleRate,
            1.0f / sampleRate,
            frameCount
        };

        for (int i=numParams; --i >=0;)
            module->params[i].setValue(*static_cast<float*>(ports[numInputs+numOutputs+i]) * 0.1f); // FIXME?

        for (uint32_t s=0; s<sampleCount; ++s)
        {
            for (int i=numInputs; --i >=0;)
                module->inputs[i].setVoltage(static_cast<const float*>(ports[i])[s] * 5.0f);

            module->doProcess(args);

            for (int i=numOutputs; --i >=0;)
                static_cast<float*>(ports[numInputs+i])[s] = module->outputs[i].getVoltage() * 0.2f;

            ++args.frame;
        }

        frameCount += sampleCount;
    }
};

static LV2_Handle lv2_instantiate(const LV2_Descriptor*, double sampleRate, const char* bundlePath, const LV2_Feature* const* features)
{
    return new PluginLv2(sampleRate);
}

// -----------------------------------------------------------------------

#define instancePtr ((PluginLv2*)instance)

static void lv2_connect_port(LV2_Handle instance, uint32_t port, void* dataLocation)
{
    instancePtr->lv2_connect_port(port, dataLocation);
}

static void lv2_activate(LV2_Handle instance)
{
    instancePtr->lv2_activate();
}

static void lv2_run(LV2_Handle instance, uint32_t sampleCount)
{
    instancePtr->lv2_run(sampleCount);
}

static void lv2_deactivate(LV2_Handle instance)
{
}

static void lv2_cleanup(LV2_Handle instance)
{
    delete instancePtr;
}

// -----------------------------------------------------------------------

static const void* lv2_extension_data(const char* uri)
{
    return nullptr;
}

#undef instancePtr

// -----------------------------------------------------------------------

static const LV2_Descriptor sLv2Descriptor = {
    PLUGIN_URI,
    lv2_instantiate,
    lv2_connect_port,
    lv2_activate,
    lv2_run,
    lv2_deactivate,
    lv2_cleanup,
    lv2_extension_data
};

DISTRHO_PLUGIN_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    USE_NAMESPACE_DISTRHO
    return (index == 0) ? &sLv2Descriptor : nullptr;
}

// -----------------------------------------------------------------------
