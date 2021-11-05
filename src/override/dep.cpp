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

#include <cstdio>
#include <cstring>

// fix blendish build, missing symbol in debug mode
#ifdef DEBUG
extern "C" {
float bnd_clamp(float v, float mn, float mx) {
    return (v > mx)?mx:(v < mn)?mn:v;
}
}
#endif

// fix bogaudio build, another missing symbol
#ifdef DEBUG
namespace bogaudio {
struct FollowerBase {
    static float efGainMaxDecibelsDebug;
};
float FollowerBase::efGainMaxDecibelsDebug = 12.0f;
}
#endif

// Compile those nice implementation-in-header little libraries
#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#undef nsvgParseFromFile
#include <nanosvg.h>

// Custom Cardinal filtering
static const struct {
    const char* filename;
    const char* shapes[4];
} pathsToFilterOut[] = {
    {
        "Core/AudioInterface.svg",
        {"path39377","path39381","path39383","path39379"}
    },
    {
        "Core/AudioInterface2.svg",
        {"path18733","path18737","path18731","path18735"}
    },
    {
        "Core/AudioInterface16.svg",
        {"path40283","path40287","path40289","path40285"}
    },
    {
        "Core/CV-CC.svg",
        {"path12881","path12885","path12887","path12883"}
    },
    {
        "Core/CV-Gate.svg",
        {"path13127","path13131","path13133","path13129"}
    },
    {
        "Core/CV-MIDI.svg",
        {"path12747","path12751","path12753","path12749"}
    },
    {
        "Core/MIDI-CC.svg",
        {"path9740","path9744","path9746","path9742"}
    },
    {
        "Core/MIDI-CV.svg",
        {"path11803","path11807","path11809","path11805"}
    },
    {
        "Core/MIDI-Gate.svg",
        {"path11634","path11638","path11640","path11636"}
    },
    {
        "Core/MIDI-Map.svg",
        {"path21209","path21213","path21215","path21211"}
    },
    {
        "Core/Notes.svg",
        {"path6935","path6939","path6941","path6937"}
    },
    {
        "Fundamental/res/8vert.svg",
        {"path69642","path69646","path69640","path69644"}
    },
    {
        "Fundamental/res/ADSR.svg",
        {"path33693","path33697","path33699","path33695"}
    },
    {
        "Fundamental/res/Delay.svg",
        {"path25369","path25373","path25375","path25371"}
    },
    {
        "Fundamental/res/LFO-1.svg",
        {"path35889","path35893","path35895","path35891"}
    },
    {
        "Fundamental/res/LFO-2.svg",
        {"path36131","path36135","path36137","path36133"}
    },
    {
        "Fundamental/res/Merge.svg",
        {"path29991","path29995","path29989","path29993"}
    },
    {
        "Fundamental/res/MidSide.svg",
        {"path44181","path44185","path44179","path44183"}
    },
    {
        "Fundamental/res/Mutes.svg",
        {"path21613","path21617","path21611","path21615"}
    },
    {
        "Fundamental/res/Noise.svg",
        {"path105594","path105598","path105592","path105596"}
    },
    {
        "Fundamental/res/Octave.svg",
        {"path38471","path38475","path38469","path38473"}
    },
    {
        "Fundamental/res/Pulses.svg",
        {"path46241","path46245","path46239","path46243"}
    },
    {
        "Fundamental/res/Quantizer.svg",
        {"path38549","path38553","path38547","path38551"}
    },
    {
        "Fundamental/res/Random.svg",
        {"path89732","path89736","path89730","path89734"}
    },
    {
        "Fundamental/res/SEQ3.svg",
        {"path35687","path35691","path35693","path35689"}
    },
    {
        "Fundamental/res/Scope.svg",
        {"path33887","path33891","path33893","path33889"}
    },
    /* These 2 do not have logos on them?
    {
        "Fundamental/res/SequentialSwitch1.svg",
        {"_______","_______","_______","_______"}
    },
    {
        "Fundamental/res/SequentialSwitch2.svg",
        {"_______","_______","_______","_______"}
    },
    */
    {
        "Fundamental/res/Split.svg",
        {"path29999","path30003","path29997","path30001"}
    },
    {
        "Fundamental/res/Sum.svg",
        {"path10913","path10917","path10911","path10915"}
    },
    {
        "Fundamental/res/Unity.svg",
        {"path21219","path21223","path21217","path21221"}
    },
    /* These 2 do not have logos on them?
    {
        "Fundamental/res/VCA-1.svg",
        {"_______","_______","_______","_______"}
    },
    {
        "Fundamental/res/VCA.svg",
        {"_______","_______","_______","_______"}
    },
    */
    {
        "Fundamental/res/VCF.svg",
        {"path25239","path25243","path25245","path25241"}
    },
    {
        "Fundamental/res/VCMixer.svg",
        {"path125839","path125843","path125845","path125841"}
    },
    {
        "Fundamental/res/VCO-1.svg",
        {"path33533","path33537","path33539","path33535"}
    },
    {
        "Fundamental/res/VCO-2.svg",
        {"path37557","path37561","path37563","path37559"}
    },
    {
        "Fundamental/res/Viz.svg",
        {"path41769","path41773","path41767","path41771"}
    },
};

static void removeShape(NSVGimage* const handle, const char* const id)
{
    for (NSVGshape *shape = handle->shapes, *old = nullptr; shape; old = shape, shape = shape->next)
    {
        if (strcmp(shape->id, id) != 0)
            continue;

        if (old != nullptr)
            old->next = shape->next;
        else
            handle->shapes = shape->next;

        nsvg__deletePaths(shape->paths);
        free(shape);
        break;
    }
}

extern "C" {
NSVGimage* nsvgParseFromFileCardinal(const char* filename, const char* units, float dpi);
}

NSVGimage* nsvgParseFromFileCardinal(const char* const filename, const char* const units, const float dpi)
{
    if (NSVGimage* const handle = nsvgParseFromFile(filename, units, dpi))
    {
        for (size_t i = 0; i < sizeof(pathsToFilterOut)/sizeof(pathsToFilterOut[0]); ++i)
        {
            const char* const pathToFilterOut = pathsToFilterOut[i].filename;
            const size_t filenamelen = std::strlen(filename);
            const size_t filterlen = std::strlen(pathToFilterOut);

            if (filenamelen < filterlen)
                continue;

            if (std::strncmp(filename + (filenamelen-filterlen), pathToFilterOut, filterlen) == 0)
            {
                puts("Removing CC-ND deadlock from file...");
                removeShape(handle, pathsToFilterOut[i].shapes[0]);
                removeShape(handle, pathsToFilterOut[i].shapes[1]);
                removeShape(handle, pathsToFilterOut[i].shapes[2]);
                removeShape(handle, pathsToFilterOut[i].shapes[3]);
            }
        }

        return handle;
    }

    return nullptr;
}
