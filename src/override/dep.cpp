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
                puts("Removing CC-ND deadlock from file...\n");
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
