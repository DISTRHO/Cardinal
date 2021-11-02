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

#include <rack.hpp>
#include <context.hpp>

#include "DistrhoUtils.hpp"

using namespace rack;

extern Model* modelSpringReverb;
Plugin* pluginInstance__Befaco;

namespace rack {
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

int main()
{
    Plugin* const p = new Plugin;
    pluginInstance__Befaco = p;
    p->addModel(modelSpringReverb);
    engine::Module* module = modelSpringReverb->createModule();

    d_stdout("modelSpringReverb is %p %p", modelSpringReverb, module);
    d_stdout("modelSpringReverb has %d ins, %d outs, %d lights, %d params",
             module->getNumInputs(), module->getNumOutputs(), module->getNumLights(), module->getNumParams());

    for (int i=0; i<module->getNumInputs(); ++i)
        d_stdout("  in %d has name '%s'; description '%s'",
                 i+1, module->getInputInfo(i)->getFullName().c_str(), module->getInputInfo(i)->getDescription().c_str());

    for (int i=0; i<module->getNumOutputs(); ++i)
        d_stdout("  out %d has name '%s'; description '%s'",
                 i+1, module->getOutputInfo(i)->getFullName().c_str(), module->getOutputInfo(i)->getDescription().c_str());

//     for (int i=0; i<module->getNumLights(); ++i)
//     {
//         LightInfo* l = module->getLightInfo(i);
//         DISTRHO_SAFE_ASSERT_CONTINUE(l != nullptr);
//         d_stdout("  light %d has name '%s'; description '%s'",
//                  i+1, l->getName().c_str(), l->getDescription().c_str());
//     }

    for (int i=0; i<module->getNumParams(); ++i)
    {
        ParamQuantity* q = module->getParamQuantity(i);
        d_stdout("  param %d has name '%s'; description '%s'; unit '%s'; min %f; max %f; def %f",
                 i+1, q->name.c_str(), q->getDescription().c_str(), q->unit.c_str(),
                 q->minValue, q->maxValue, q->defaultValue);
    }

    Module::ProcessArgs args = {
        48000.0f,
        1.0f / 48000.0f,
        0
    };
    for (int i=0; i<96000; ++i)
    {
        module->process(args);
        ++args.frame;
    }
    return 0;
}
