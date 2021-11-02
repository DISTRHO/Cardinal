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

#include "src/lv2/buf-size.h"
#include "src/lv2/options.h"
#include <rack.hpp>
#include <context.hpp>

#include "DistrhoUtils.hpp"

using namespace rack;

extern Model* modelSpringReverb;
Plugin* pluginInstance__Befaco;

namespace rack {
namespace settings {
bool cpuMeter = false;
}
Context::~Context() {
}
static thread_local Context* threadContext;
Context* contextGet() {
	DISTRHO_SAFE_ASSERT(threadContext != nullptr);
	return threadContext;
}
// Apple's clang incorrectly compiles this function when -O2 or higher is enabled.
#ifdef ARCH_MAC
__attribute__((optnone))
#endif
void contextSet(Context* const context) {
	// DISTRHO_SAFE_ASSERT(threadContext == nullptr);
	threadContext = context;
}
Exception::Exception(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    msg = string::fV(format, args);
    va_end(args);
}
namespace asset {
std::string plugin(plugin::Plugin* plugin, std::string filename) { return {}; }
std::string system(std::string filename) { return {}; }
}
namespace engine {
float Engine::getParamValue(Module* module, int paramId) { return 0.0f; }
float Engine::getParamSmoothValue(Module* module, int paramId) { return 0.0f; }
void Engine::setParamValue(Module* module, int paramId, float value) {}
void Engine::setParamSmoothValue(Module* module, int paramId, float value) {}
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
    Plugin* plugin;
    engine::Module* module;
    float sampleRate;
    int frameCount = 0;

    void* ports[11];

    PluginLv2(double sr)
    {
        sampleRate = sr;
        plugin = new Plugin;
        pluginInstance__Befaco = plugin;
        plugin->addModel(modelSpringReverb);
        module = modelSpringReverb->createModule();

        // FIXME we need to detect if something is connected
        // module->inputs[0].channels = 1;
        // module->inputs[1].channels = 1;
        module->inputs[2].channels = 1;
        module->inputs[3].channels = 1;
        module->inputs[4].channels = 1;
        module->outputs[0].channels = 1;
        module->outputs[1].channels = 1;
    }

    void lv2_connect_port(const uint32_t port, void* const dataLocation)
    {
        ports[port] = dataLocation;
    }

    void lv2_run(const uint32_t sampleCount)
    {
        if (sampleCount == 0)
            return;

        Module::ProcessArgs args = {
            sampleRate,
            1.0f / sampleRate,
            frameCount
        };

        // const float* CV1_INPUT = (float*)ports[0];
        // const float* CV2_INPUT = (float*)ports[1];
        const float* IN1_INPUT = (float*)ports[2];
        const float* IN2_INPUT = (float*)ports[3];
        const float* MIX_CV_INPUT = (float*)ports[4];
        float* MIX_OUTPUT = (float*)ports[5];
        float* WET_OUTPUT = (float*)ports[6];

        const float drywet = *(float*)ports[7] * 0.01f;
        const float lvl1 = *(float*)ports[8] * 0.01f;
        const float lvl2 = *(float*)ports[9] * 0.01f;
        const float hpf = *(float*)ports[10];

        module->params[0].setValue(drywet);
        module->params[1].setValue(lvl1);
        module->params[2].setValue(lvl2);
        module->params[3].setValue(hpf);

        for (uint32_t i=0; i<sampleCount; ++i)
        {
            // module->inputs[0].setVoltage(CV1_INPUT[i]);
            // module->inputs[1].setVoltage(CV2_INPUT[i]);
            module->inputs[2].setVoltage(IN1_INPUT[i] * 10);
            module->inputs[3].setVoltage(IN2_INPUT[i] * 10);
            module->inputs[4].setVoltage(MIX_CV_INPUT[i]);
            module->doProcess(args);
            MIX_OUTPUT[i] = module->outputs[0].getVoltage() * 0.1f;
            WET_OUTPUT[i] = module->outputs[1].getVoltage() * 0.1f;
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
    "urn:Cardinal:Befaco",
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
