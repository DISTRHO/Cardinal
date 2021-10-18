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

#pragma once

#include <audio.hpp>
#include <context.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class CardinalBasePlugin : public Plugin {
public:
    CardinalBasePlugin(uint32_t parameterCount, uint32_t programCount, uint32_t stateCount)
        : Plugin(parameterCount, programCount, stateCount) {}
    ~CardinalBasePlugin() override {}
    virtual bool isActive() const noexcept = 0;
    virtual bool canAssignDevice() const noexcept = 0;
    virtual void assignDevice(rack::audio::Device* dev) noexcept = 0;
    virtual bool clearDevice(rack::audio::Device* dev) noexcept = 0;
};

// -----------------------------------------------------------------------------------------------------------

struct CardinalPluginContext : rack::Context {
    CardinalBasePlugin* const plugin;

    CardinalPluginContext(CardinalBasePlugin* const p)
        : plugin(p) {}
};

// -----------------------------------------------------------------------------------------------------------

struct CardinalAudioDevice : rack::audio::Device {
    CardinalBasePlugin* const fPlugin;

    CardinalAudioDevice(CardinalBasePlugin* const plugin)
        : fPlugin(plugin) {}

    std::string getName() override
    {
        return "Cardinal";
    }

    int getNumInputs() override
    {
        return DISTRHO_PLUGIN_NUM_INPUTS;
    }

    int getNumOutputs() override
    {
        return DISTRHO_PLUGIN_NUM_OUTPUTS;
    }

    int getBlockSize() override
    {
        return fPlugin->getBufferSize();
    }

    float getSampleRate() override
    {
        return fPlugin->getSampleRate();
    }

    std::set<int> getBlockSizes() override
    {
        return std::set<int>({ getBlockSize() });
    }

    std::set<float> getSampleRates() override
    {
        return std::set<float>({ getSampleRate() });
    }

    void setBlockSize(int) override {}
    void setSampleRate(float) override {}
};

// -----------------------------------------------------------------------------------------------------------

struct CardinalAudioDriver : rack::audio::Driver {

    CardinalAudioDriver() {}

    std::string getName() override
    {
        return "Plugin Driver";
    }

    std::vector<int> getDeviceIds() override
    {
        return std::vector<int>({ 0 });
    }

    std::string getDeviceName(int) override
    {
        return "Plugin Device";
    }

    int getDeviceNumInputs(int) override
    {
        return DISTRHO_PLUGIN_NUM_INPUTS;
    }

    int getDeviceNumOutputs(int) override
    {
        return DISTRHO_PLUGIN_NUM_OUTPUTS;
    }

    rack::audio::Device* subscribe(int, rack::audio::Port* const port) override
    {
        CardinalPluginContext* const pluginContext = reinterpret_cast<CardinalPluginContext*>(port->context);
        DISTRHO_SAFE_ASSERT_RETURN(pluginContext != nullptr, nullptr);

        CardinalBasePlugin* const plugin = pluginContext->plugin;
        DISTRHO_SAFE_ASSERT_RETURN(plugin != nullptr, nullptr);

        if (! plugin->canAssignDevice())
            throw rack::Exception("Plugin driver only allows one audio device to be used simultaneously");

        CardinalAudioDevice* const device = new CardinalAudioDevice(plugin);
        device->subscribe(port);

        if (plugin->isActive())
            device->onStartStream();

        plugin->assignDevice(device);
        return device;
    }

    void unsubscribe(int, rack::audio::Port* const port) override
    {
        CardinalAudioDevice* const device = reinterpret_cast<CardinalAudioDevice*>(port->device);
        DISTRHO_SAFE_ASSERT_RETURN(device != nullptr,);

        CardinalPluginContext* const pluginContext = reinterpret_cast<CardinalPluginContext*>(port->context);
        DISTRHO_SAFE_ASSERT_RETURN(pluginContext != nullptr,);

        CardinalBasePlugin* const plugin = pluginContext->plugin;
        DISTRHO_SAFE_ASSERT_RETURN(plugin != nullptr,);

        if (plugin->clearDevice(device))
        {
            device->onStopStream();
            device->unsubscribe(port);
            delete device;
        }
    }
};

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
