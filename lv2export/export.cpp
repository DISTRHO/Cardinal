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

#include <cstdio>

// -----------------------------------------------------------------------

DISTRHO_PLUGIN_EXPORT
void lv2_generate_ttl()
{
    Context context;
    context._engine.sampleRate = 48000.f;
    contextSet(&context);

    engine::Module* module = PLUGIN_MODEL->createModule();

    d_stdout("@prefix doap:  <http://usefulinc.com/ns/doap#> .");
    d_stdout("@prefix foaf:  <http://xmlns.com/foaf/0.1/> .");
    d_stdout("@prefix lv2:   <http://lv2plug.in/ns/lv2core#> .");
    d_stdout("");

    d_stdout("<urn:cardinal:" SLUG ">");
    d_stdout("    a lv2:Plugin, doap:Project ;");
    d_stdout("    doap:name \"" SLUG "\" ;");
    d_stdout("");

    int index = 0;

    for (int i=0, numAudio=0, numCV=0; i<module->getNumInputs(); ++i)
    {
        d_stdout("    lv2:port [");
        if (kCvInputs[i])
        {
            d_stdout("        a lv2:InputPort, lv2:CVPort ;");
            d_stdout("        lv2:symbol \"lv2_cv_in_%d\" ;", ++numCV);
        }
        else
        {
            d_stdout("        a lv2:InputPort, lv2:AudioPort ;");
            d_stdout("        lv2:symbol \"lv2_audio_in_%d\" ;", ++numAudio);
        }
        d_stdout("        lv2:name \"%s\" ;", module->getInputInfo(i)->getFullName().c_str());
        d_stdout("        lv2:index %d ;", index++);
        d_stdout("    ] ;");
        d_stdout("");
    }

    for (int i=0, numAudio=0, numCV=0; i<module->getNumOutputs(); ++i)
    {
        d_stdout("    lv2:port [");
        if (kCvOutputs[i])
        {
            d_stdout("        a lv2:OutputPort, lv2:CVPort ;");
            d_stdout("        lv2:symbol \"lv2_cv_out_%d\" ;", ++numCV);
        }
        else
        {
            d_stdout("        a lv2:OutputPort, lv2:AudioPort ;");
            d_stdout("        lv2:symbol \"lv2_audio_out_%d\" ;", ++numAudio);
        }
        d_stdout("        lv2:name \"%s\" ;", module->getOutputInfo(i)->getFullName().c_str());
        d_stdout("        lv2:index %d ;", index++);
        d_stdout("    ] ;");
        d_stdout("");
    }

    for (int i=0; i<module->getNumParams(); ++i)
    {
        ParamQuantity* q = module->getParamQuantity(i);
        d_stdout("    lv2:port [");
        d_stdout("        a lv2:InputPort, lv2:ControlPort ;");
        d_stdout("        lv2:index %d ;", index++);
        d_stdout("        lv2:symbol \"lv2_param_%d\" ;", i + 1);
        d_stdout("        lv2:name \"%s\" ;", q->name.c_str());
        d_stdout("        lv2:default %f ;", q->defaultValue);
        d_stdout("        lv2:minimum %f ;", q->minValue);
        d_stdout("        lv2:maximum %f ;", q->maxValue);
        d_stdout("    ] ;");
        d_stdout("");
        // q->getDescription().c_str()
        // q->unit.c_str()
    }

    d_stdout("    .");

    delete module;
}

// -----------------------------------------------------------------------
