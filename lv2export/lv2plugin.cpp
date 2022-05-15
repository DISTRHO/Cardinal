/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
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

#ifndef PLUGIN_MODEL
# error PLUGIN_MODEL undefined
#endif

#ifndef PLUGIN_CV_INPUTS
# error PLUGIN_CV_INPUTS undefined
#endif

#ifndef PLUGIN_CV_OUTPUTS
# error PLUGIN_CV_OUTPUTS undefined
#endif

static constexpr const bool kCvInputs[] = PLUGIN_CV_INPUTS;
static constexpr const bool kCvOutputs[] = PLUGIN_CV_OUTPUTS;

#include "src/lv2/buf-size.h"
#include "src/lv2/options.h"

#include "DistrhoUtils.hpp"

namespace rack {

static thread_local Context* threadContext = nullptr;

Context* contextGet() {
    DISTRHO_SAFE_ASSERT(threadContext != nullptr);
    return threadContext;
}

#ifdef ARCH_MAC
__attribute__((optnone))
#endif
void contextSet(Context* context) {
    threadContext = context;
}

}

struct PluginLv2 {
    Context context;
    engine::Module* module;
    int frameCount = 0;
    int numInputs, numOutputs, numParams, numLights;
    void** ports;

    PluginLv2(double sr)
    {
        context._engine.sampleRate = sr;
        contextSet(&context);
        module = PLUGIN_MODEL->createModule();

        numInputs = module->getNumInputs();
        numOutputs = module->getNumOutputs();
        numParams = module->getNumParams();
        numLights = module->getNumLights();
        ports = new void*[numInputs+numOutputs+numParams+numLights];

        Module::SampleRateChangeEvent e = { context._engine.sampleRate, 1.0f / context._engine.sampleRate };
        module->onSampleRateChange(e);

        // FIXME for CV ports we need to detect if something is connected
        for (int i=numInputs; --i >=0;)
        {
            if (!kCvInputs[i])
                module->inputs[i].channels = 1;
        }
        for (int i=numOutputs; --i >=0;)
        {
            if (!kCvOutputs[i])
                module->outputs[i].channels = 1;
        }

        d_stdout("Loaded " SLUG " :: %i inputs, %i outputs, %i params and %i lights",
                 numInputs, numOutputs, numParams, numLights);
    }

    PluginLv2()
    {
        contextSet(&context);
        delete[] ports;
        delete module;
    }

    void lv2_connect_port(const uint32_t port, void* const dataLocation)
    {
        ports[port] = dataLocation;
    }

    void lv2_activate()
    {
        contextSet(&context);
        module->onReset();
    }

    void lv2_run(const uint32_t sampleCount)
    {
        if (sampleCount == 0)
            return;

        contextSet(&context);

        Module::ProcessArgs args = { context._engine.sampleRate, 1.0f / context._engine.sampleRate, frameCount };

        for (int i=numParams; --i >=0;)
            module->params[i].setValue(*static_cast<const float*>(ports[numInputs+numOutputs+i]));

        for (uint32_t s=0; s<sampleCount; ++s)
        {
            for (int i=numInputs; --i >=0;)
            {
                if (kCvInputs[i])
                    module->inputs[i].setVoltage(static_cast<const float*>(ports[i])[s]);
                else
                    module->inputs[i].setVoltage(static_cast<const float*>(ports[i])[s] * 10.0f);
            }

            module->doProcess(args);

            for (int i=numOutputs; --i >=0;)
            {
                if (kCvOutputs[i])
                    static_cast<float*>(ports[numInputs+i])[s] = module->outputs[i].getVoltage();
                else
                    static_cast<float*>(ports[numInputs+i])[s] = module->outputs[i].getVoltage() * 0.1f;
            }

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
    "urn:cardinal:" SLUG,
    lv2_instantiate,
    lv2_connect_port,
    lv2_activate,
    lv2_run,
    NULL, // deactivate
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
