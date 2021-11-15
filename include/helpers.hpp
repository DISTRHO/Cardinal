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

/**
 * This file is an edited version of VCVRack's helpers.hpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#pragma once

#include <app/ModuleWidget.hpp>
#include <engine/Module.hpp>

#include <unordered_map>

namespace rack {

struct CardinalPluginModelHelper {
    virtual ~CardinalPluginModelHelper() {}
    virtual void createCachedModuleWidget(rack::engine::Module* m) = 0;
    virtual void clearCachedModuleWidget(rack::engine::Module* m) = 0;
};

template <class TModule, class TModuleWidget>
struct CardinalPluginModel : plugin::Model, CardinalPluginModelHelper
{
    std::unordered_map<rack::engine::Module*, app::ModuleWidget*> widgets;

    rack::engine::Module* createModule() override
    {
        engine::Module* const m = new TModule;
        m->model = this;
        return m;
    }

    app::ModuleWidget* createModuleWidget(rack::engine::Module* const m) override
    {
        TModule* tm = NULL;
        if (m) {
            assert(m->model == this);
            if (widgets.find(m) != widgets.end())
                return widgets[m];
            tm = dynamic_cast<TModule*>(m);
        }
        app::ModuleWidget* mw = new TModuleWidget(tm);
        mw->setModel(this);
        return mw;
    }

    void createCachedModuleWidget(rack::engine::Module* const m) override
    {
        assert(m != nullptr); if (m == nullptr) return;
        assert(m->model == this); if (m->model != this) return;
        TModule* const tm = dynamic_cast<TModule*>(m);
        TModuleWidget* const mw = new TModuleWidget(tm);
        mw->setModel(this);
        widgets[m] = mw;
    }

    void clearCachedModuleWidget(rack::engine::Module* const m) override
    {
        assert(m != nullptr); if (m == nullptr) return;
        assert(m->model == this); if (m->model != this) return;
        widgets.erase(m);
    }
};

template <class TModule, class TModuleWidget>
CardinalPluginModel<TModule, TModuleWidget>* createModel(std::string slug)
{
	CardinalPluginModel<TModule, TModuleWidget>* const o = new CardinalPluginModel<TModule, TModuleWidget>();
	o->slug = slug;
	return o;
}

}

#define createModel createModelOldVCV
#include_next "helpers.hpp"
