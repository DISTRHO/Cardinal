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

#include "DistrhoUtils.hpp"

namespace rack {

struct CardinalPluginModelHelper : plugin::Model {
    virtual app::ModuleWidget* createModuleWidgetFromEngineLoad(engine::Module* m) = 0;
    virtual void removeCachedModuleWidget(engine::Module* m) = 0;
};

template <class TModule, class TModuleWidget>
struct CardinalPluginModel : CardinalPluginModelHelper
{
    std::unordered_map<engine::Module*, TModuleWidget*> widgets;
    std::unordered_map<engine::Module*, bool> widgetNeedsDeletion;

    engine::Module* createModule() override
    {
        engine::Module* const m = new TModule;
        m->model = this;
        return m;
    }

    app::ModuleWidget* createModuleWidget(engine::Module* const m) override
    {
        TModule* tm = nullptr;
        if (m)
        {
            DISTRHO_SAFE_ASSERT_RETURN(m->model == this, nullptr);
            if (widgets.find(m) != widgets.end())
            {
                widgetNeedsDeletion[m] = false;
                return widgets[m];
            }
            tm = dynamic_cast<TModule*>(m);
        }
        app::ModuleWidget* const tmw = new TModuleWidget(tm);
        DISTRHO_SAFE_ASSERT_RETURN(tmw->module == m, nullptr);
        tmw->setModel(this);
        return tmw;
    }

    app::ModuleWidget* createModuleWidgetFromEngineLoad(engine::Module* const m) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(m != nullptr, nullptr);
        DISTRHO_SAFE_ASSERT_RETURN(m->model == this, nullptr);

        TModule* const tm = dynamic_cast<TModule*>(m);
        DISTRHO_SAFE_ASSERT_RETURN(tm != nullptr, nullptr);

        TModuleWidget* const tmw = new TModuleWidget(tm);
        DISTRHO_SAFE_ASSERT_RETURN(tmw->module == m, nullptr);
        tmw->setModel(this);

        widgets[m] = tmw;
        widgetNeedsDeletion[m] = true;
        return tmw;
    }

    void removeCachedModuleWidget(engine::Module* const m) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(m != nullptr,);
        DISTRHO_SAFE_ASSERT_RETURN(m->model == this,);

        if (widgets.find(m) == widgets.end())
            return;

        if (widgetNeedsDeletion[m])
            delete widgets[m];

        widgets.erase(m);
        widgetNeedsDeletion.erase(m);
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
#undef createModel

// this is defined by Windows headers, conflicts with Bidoo VOID module name
#undef VOID
