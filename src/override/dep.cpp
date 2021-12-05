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

#define STDIO_OVERRIDE Rackdep

#include <cstdio>
#include <cstring>

// fix blendish build, missing symbol in debug mode
#ifndef NDEBUG
extern "C" {
float bnd_clamp(float v, float mn, float mx) {
    return (v > mx)?mx:(v < mn)?mn:v;
}
}
#endif

// fix bogaudio build, another missing symbol
#ifndef NDEBUG
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
    const char* const filename;
    const char* shapes[8];
} pathsToFilterOut[] = {
    {
        "/Core/Audio2.svg",
        {
            "af0e935f-6d5d-444c-895d-4ef87ed070e2",
            "a86c2c4c-97ac-4697-a68a-6850586e2c02",
            "bcf7f738-a2ee-4f24-a05e-79080795ecac",
            "a97fec2c-c9d1-4f34-979d-c2ed13bb4b13"
        }
    },
    {
        "/Core/Audio8.svg",
        {
            "a0f6228c-5a83-4e0f-aae7-549956d6c592",
            "ab26ff62-9ae1-4a5d-a800-ae8a9605590e",
            "a5905f87-4917-4633-8c97-2c26610c8dd4",
            "b9c0a0ed-e7c4-4727-8d9a-2f8d91fb0687"
        }
    },
    {
        "/Core/Audio16.svg",
        {
            "b1f50144-7f61-4995-a84e-7c733562b87a",
            "bc1bc3f6-2589-483f-a045-f4d5b20fb1bb",
            "aa22e3a2-cd01-4140-bbb6-d7011a95a8a2",
            "ea03e764-3d44-4889-a575-96635de1afbe"
        }
    },
    {
        "/Core/CV_MIDI.svg",
        {
            "bccecf5a-8b5e-488a-8cf7-650646675413",
            "e23862c8-ee12-4114-8f76-5ec17e2f6556",
            "ecee65f0-2e8f-465d-a3f5-7c42fd129ac0",
            "e5f4878f-ccca-4a7b-9150-00a37e0f7a74"
        }
    },
    {
        "/Core/CV_MIDICC.svg",
        {
            "a00f3f95-93a1-4715-a488-46304d54fe4f",
            "a08adb03-865c-43cd-8cb5-971a122dc23b",
            "acc2b450-a2fc-418e-a77e-156a2488ec7a",
            "e06020ad-ee93-4ba3-893c-67c2998ab6cb"
        }
    },
    {
        "/Core/Gate_MIDI.svg",
        {
            "f2d57f2b-4ac3-4488-87ca-eb36011b7ce2",
            "bc89b1d7-8b88-4c7a-af75-dfc0ea49931b",
            "a11340bf-b366-408c-b4b3-38117f4076dc",
            "aa3e2e62-1159-407f-836f-d2ff19be0436"
        }
    },
    {
        "/Core/MIDI_CV.svg",
        {
            "fca6f6f9-431a-4f4f-9eb9-85798d95f9ff",
            "a520f07d-0c80-4572-aabf-aa329a17c02b",
            "b824fab4-8a48-404a-b94f-d27f4404ee73",
            "beded7b3-65dd-4587-af53-097e8e4afb82"
        }
    },
    {
        "/Core/MIDI_Gate.svg",
        {
            "fb3f5bb1-31c5-4a85-95ef-a09d670df926",
            "a32f20f9-d55f-46ad-b898-7dc335b20780",
            "bf8ce0d6-d9b5-4e48-bf72-dd6ddd7e4866",
            "ba9e0012-4a65-4add-bd98-ba3570579a83"
        }
    },
    {
        "/Core/MIDICC_CV.svg",
        {
            "bc30b86e-2d28-4d36-a374-69ba5693476f",
            "ffe265e3-2ac1-4b6b-8361-c3aaecd0b08f",
            "e822d5df-0aee-4ddb-9c3d-a6878d3d06e2",
            "b4f0f926-0fae-49be-b7d9-5bd02e5798af"
        }
    },
    {
        "/Core/MIDIMap.svg",
        {
            "b3405776-74b5-4087-a983-4efe1be00b3e",
            "bd42b57c-2a9e-4cc7-a268-70bc560923d7",
            "e2520435-d4b5-45d8-8264-c033ec31cfb4",
            "f774ab20-064d-4751-baa5-11632dd31e08"
        }
    },
    {
        "/Core/Notes.svg",
        {
            "b4cb817a-ff42-4dfc-8866-e3d83608b518",
            "aa48af94-af91-4b1f-b663-ef6af4b5cf3b",
            "a3b43b3b-5987-49ba-834a-210c6d95bb7d",
            "a58877fa-d57b-4d74-bbda-a883bf30a365"
        }
    },
    {
        "/Fundamental/8vert.svg",
        {"path17","path21","circle15","path19"}
    },
    {
        "/Fundamental/ADSR.svg",
        {"path12","path16","circle18","path14"}
    },
    {
        "/Fundamental/Delay.svg",
        {"path14","path18","circle20","path16"}
    },
    {
        "/Fundamental/LFO.svg",
        {"path14","path18","circle20","path16"}
    },
    {
        "/Fundamental/Merge.svg",
        {"path16","path20","circle14","path18",
         "path26","path30","circle24","path28"}
    },
    {
        "/Fundamental/MidSide.svg",
        {"path23","path27","circle21","path25",
         "path33","path37","circle31","path35"}
    },
    {
        "/Fundamental/Mixer.svg",
        {"path14","path18","circle12","path16"}
    },
    {
        "/Fundamental/Mutes.svg",
        {"path17","path21","circle15","path19"}
    },
    {
        "/Fundamental/Noise.svg",
        {"path14","path18","circle12","path16"}
    },
    {
        "/Fundamental/Octave.svg",
        {"path14","path18","circle12","path16"}
    },
    {
        "/Fundamental/Pulses.svg",
        {"path14","path18","circle12","path16"}
    },
    {
        "/Fundamental/Quantizer.svg",
        {"path14","path18","circle12","path16"}
    },
    {
        "/Fundamental/Random.svg",
        {"path58","path62","circle64","path60"}
    },
    {
        "/Fundamental/Scope.svg",
        {"path14","path18","circle20","path16"}
    },
    {
        "/Fundamental/SEQ3.svg",
        {"path16","path20","circle22","path18"}
    },
    {
        "/Fundamental/SequentialSwitch1.svg",
        {"path17","path21","circle15","path19"}
    },
    {
        "/Fundamental/SequentialSwitch2.svg",
        {"path17","path21","circle15","path19"}
    },
    {
        "/Fundamental/Split.svg",
        {"path18","path22","circle16","path20",
         "path28","path32","circle26","path30"}
    },
    {
        "/Fundamental/Sum.svg",
        {"path24","path28","circle22","path26"}
    },
    {
        "/Fundamental/Unity.svg",
        {"path21219","path21223","path21217","path21221"}
    },
    {
        "/Fundamental/VCA-1.svg",
        {"path16","path20","circle14","path18"}
    },
    {
        "/Fundamental/VCF.svg",
        {"path12","path16","circle18","path14"}
    },
    {
        "/Fundamental/VCMixer.svg",
        {"path12","path16","circle18","path14"}
    },
    {
        "/Fundamental/VCO.svg",
        {"path14","path18","circle20","path16"}
    },
    {
        "/Fundamental/Viz.svg",
        {"path14","path18","circle12","path16"}
    },
    {
        "/Fundamental/WTLFO.svg",
        {"path12","path16","circle18","path14"}
    },
    {
        "/Fundamental/WTVCO.svg",
        {"path12","path16","circle18","path14"}
    },
};
static const struct {
    const char* const filename;
    const char* shapeIdsToIgnore[5];
    const int shapeNumberToIgnore;
} svgFilesToInvert[] = {
    { "/AnimatedCircuits/FoldingLight.svg", {}, -1 },
    { "/AnimatedCircuits/Knob_Black_Light_21.svg", {}, -1 },
    { "/AudibleInstruments/Blinds.svg", {}, -1 },
    { "/AudibleInstruments/Braids.svg", {}, -1 },
    { "/AudibleInstruments/Branches.svg", {}, -1 },
    { "/AudibleInstruments/Clouds.svg", {}, -1 },
    { "/AudibleInstruments/Elements.svg", {}, -1 },
    { "/AudibleInstruments/Frames.svg", {}, -1 },
    { "/AudibleInstruments/Kinks.svg", {}, -1 },
    { "/AudibleInstruments/Links.svg", {}, -1 },
    { "/AudibleInstruments/Marbles.svg", {}, -1 },
    { "/AudibleInstruments/Peaks.svg", {}, -1 },
    { "/AudibleInstruments/Plaits.svg", {}, -1 },
    { "/AudibleInstruments/Rings.svg", {}, -1 },
    { "/AudibleInstruments/Ripples.svg", {}, -1 },
    { "/AudibleInstruments/Shades.svg", {}, -1 },
    { "/AudibleInstruments/Sheep.svg", {}, -1 },
    { "/AudibleInstruments/Shelves.svg", {}, -1 },
    { "/AudibleInstruments/Stages.svg", {}, -1 },
    { "/AudibleInstruments/Streams.svg", {}, -1 },
    { "/AudibleInstruments/Tides.svg", {}, -1 },
    { "/AudibleInstruments/Tides2.svg", {}, -1 },
    { "/AudibleInstruments/Veils.svg", {}, -1 },
    { "/AudibleInstruments/Warps.svg", {}, -1 },
    { "/Bidoo/ACNE.svg", {}, -1 },
    { "/Bidoo/ANTN.svg", {}, -1 },
    { "/Bidoo/BAFIS.svg", {}, -1 },
    { "/Bidoo/BANCAU.svg", {}, -1 },
    { "/Bidoo/BAR.svg", {"rect833"}, -1 },
    { "/Bidoo/BISTROT.svg", {}, -1 },
    { "/Bidoo/BORDL.svg", {"rect959-3-0-7-5","rect959-3-0-7","rect959-3-0","rect959-3"}, -1 },
    { "/Bidoo/CANARD.svg", {"rect959-3-7"}, -1 },
    { "/Bidoo/CHUTE.svg", {}, -1 },
    { "/Bidoo/DFUZE.svg", {}, -1 },
    { "/Bidoo/DIKTAT.svg", {"rect843","rect843-0","rect843-0-8"}, -1 },
    { "/Bidoo/DILEMO.svg", {}, -1 },
    { "/Bidoo/DTROY.svg", {"rect959-3"}, -1 },
    { "/Bidoo/DUKE.svg", {}, -1 },
    { "/Bidoo/EDSAROS.svg", {"rect959-3-7","rect959-3-7-8","rect959-3-7-8-1","rect959-3-7-8-1-4"}, -1 },
    { "/Bidoo/EMILE.svg", {}, -1 },
    { "/Bidoo/FLAME.svg", {}, -1 },
    { "/Bidoo/FORK.svg", {}, -1 },
    { "/Bidoo/FREIN.svg", {}, -1 },
    { "/Bidoo/HCTIP.svg", {}, -1 },
    { "/Bidoo/HUITRE.svg", {}, -1 },
    { "/Bidoo/LAMBDA.svg", {}, -1 },
    { "/Bidoo/LATE.svg", {}, -1 },
    { "/Bidoo/LIMBO.svg", {}, -1 },
    { "/Bidoo/LIMONADE.svg", {"rect839","rect839-6"}, -1 },
    { "/Bidoo/LOURDE.svg", {"rect847","rect847-7","rect847-5","rect847-3"}, -1 },
    { "/Bidoo/MAGMA.svg", {}, -1 },
    { "/Bidoo/MINIBAR.svg", {"rect833"}, -1 },
    { "/Bidoo/MOIRE.svg", {"rect843","rect843-7"}, -1 },
    { "/Bidoo/MS.svg", {}, -1 },
    { "/Bidoo/MU.svg", {"rect864"}, -1 },
    { "/Bidoo/OAI.svg", {}, -1 },
    { "/Bidoo/OUAIVE.svg", {"rect959-3-7"}, -1 },
    { "/Bidoo/PERCO.svg", {}, -1 },
    { "/Bidoo/PILOT.svg", {"rect843-6-4-5","rect843","rect843-4","rect843-6-4","rect843-6-7"}, -1 },
    { "/Bidoo/POUPRE.svg", {}, -1 },
    { "/Bidoo/RABBIT.svg", {}, -1 },
    { "/Bidoo/REI.svg", {}, -1 },
    { "/Bidoo/SIGMA.svg", {}, -1 },
    { "/Bidoo/SPORE.svg", {}, -1 },
    { "/Bidoo/TIARE.svg", {}, -1 },
    { "/Bidoo/TOCANTE.svg", {"rect843"}, -1 },
    { "/Bidoo/VOID.svg", {}, -1 },
    { "/Bidoo/ZINC.svg", {}, -1 },
    { "/Bidoo/ZOUMAI.svg", {}, -1 },
    { "/cf/ALGEBRA.svg", {}, -1 },
    { "/cf/BUFFER.svg", {}, -1 },
    { "/cf/CHOKE.svg", {}, -1 },
    { "/cf/CUBE.svg", {}, -1 },
    { "/cf/CUTS.svg", {}, -1 },
    { "/cf/DISTO.svg", {}, -1 },
    { "/cf/EACH.svg", {}, -1 },
    { "/cf/FOUR.svg", {}, -1 },
    { "/cf/FUNKTION.svg", {}, -1 },
    { "/cf/L3DS3Q.svg", {}, 3 },
    { "/cf/LABEL.svg", {}, -1 },
    { "/cf/LEDS.svg", {}, -1 },
    { "/cf/LEDSEQ.svg", {}, 3 },
    { "/cf/MASTER.svg", {}, -1 },
    { "/cf/METRO.svg", {}, -1 },
    { "/cf/MONO.svg", {}, -1 },
    { "/cf/PATCH.svg", {}, -1 },
    { "/cf/PEAK.svg", {}, -1 },
    { "/cf/PLAY.svg", {}, -1 },
    { "/cf/PLAYER.svg", {}, -1 },
    { "/cf/SLIDERSEQ.svg", {}, -1 },
    { "/cf/STEPS.svg", {}, -1 },
    { "/cf/STEREO.svg", {}, -1 },
    { "/cf/SUB.svg", {}, -1 },
    { "/cf/trSEQ.svg", {}, -1 },
    { "/cf/VARIABLE.svg", {}, -1 },
    { "/DrumKit/Baronial.svg", {}, -1 },
    { "/DrumKit/BD9.svg", {}, -1 },
    { "/DrumKit/ClosedHH.svg", {}, -1 },
    { "/DrumKit/CR78.svg", {}, -1 },
    { "/DrumKit/DMX.svg", {}, -1 },
    { "/DrumKit/Gnome.svg", {}, -1 },
    { "/DrumKit/Marionette.svg", {}, -1 },
    { "/DrumKit/OpenHH.svg", {}, -1 },
    { "/DrumKit/SBD.svg", {}, -1 },
    { "/DrumKit/Sequencer.svg", {}, -1 },
    { "/DrumKit/Snare.svg", {}, -1 },
    { "/DrumKit/Tomi.svg", {}, -1 },
    { "/ESeries/E340.svg", {}, -1 },
    { "/Fundamental/VCA.svg", {}, -1 },
    { "/JW-Modules/Add5.svg", {}, -1 },
    { "/JW-Modules/BlankPanel1hp.svg", {}, -1 },
    { "/JW-Modules/BlankPanelLarge.svg", {}, -1 },
    { "/JW-Modules/BlankPanelMedium.svg", {}, -1 },
    { "/JW-Modules/BlankPanelSmall.svg", {}, -1 },
    { "/JW-Modules/BouncyBalls.svg", {}, -1 },
    { "/JW-Modules/D1v1de.svg", {}, -1 },
    { "/JW-Modules/DivSeq.svg", {}, -1 },
    { "/JW-Modules/EightSeq.svg", {}, -1 },
    { "/JW-Modules/GridSeq.svg", {}, -1 },
    { "/JW-Modules/MinMax.svg", {"path38411"}, -1 },
    { "/JW-Modules/NoteSeq.svg", {}, -1 },
    { "/JW-Modules/NoteSeq16.svg", {}, -1 },
    { "/JW-Modules/NoteSeqFu.svg", {}, -1 },
    { "/JW-Modules/OnePattern.svg", {}, -1 },
    { "/JW-Modules/Patterns.svg", {}, -1 },
    { "/JW-Modules/Pres1t.svg", {}, -1 },
    { "/JW-Modules/PT.svg", {}, -1 },
    { "/JW-Modules/Str1ker.svg", {"rect2094","rect995","rect169"}, -1 },
    { "/JW-Modules/Trigs.svg", {}, -1 },
    { "/JW-Modules/WavHeadPanel.svg", {}, -1 },
    { "/JW-Modules/XYPad.svg", {}, -1 },
};

static inline void removeShape(NSVGimage* const handle, const char* const id)
{
    if (id == nullptr)
        return;

    for (NSVGshape *shape = handle->shapes, *old = nullptr; shape != nullptr; old = shape, shape = shape->next)
    {
        if (std::strcmp(shape->id, id) != 0)
            continue;

        if (old != nullptr)
            old->next = shape->next;
        else
            handle->shapes = shape->next;

        nsvg__deletePaths(shape->paths);
        free(shape);
        return;
    }

    printf("NOTICE: failed to find '%s' shape to remove\n", id);
}

static inline bool invertPaint(NSVGpaint& paint, const char* const svgFileToInvert = nullptr)
{
    // Special case for DrumKit background grandient
    if (paint.type == NSVG_PAINT_LINEAR_GRADIENT && svgFileToInvert != nullptr && std::strncmp(svgFileToInvert, "/DrumKit/", 9) == 0)
    {
        paint.type = NSVG_PAINT_COLOR;
        paint.color = 0xff191919;
        return true;
    }

    if (paint.type == NSVG_PAINT_NONE)
        return true;
    if (paint.type != NSVG_PAINT_COLOR)
        return false;

    // Special case for Bidoo red color
    if (paint.color == 0xff001fcd && svgFileToInvert != nullptr && std::strncmp(svgFileToInvert, "/Bidoo/", 7) == 0)
    {
        paint.color = 0xcf8b94c4;
        return true;
    }

    // Special case for JW-Modules colors
    if (svgFileToInvert != nullptr && std::strncmp(svgFileToInvert, "/JW-Modules/", 12) == 0)
    {
        switch (paint.color)
        {
        // do nothing
        case 0x320997ff:
        case 0x32099aff:
        case 0x3209f1ff:
        case 0x3209f3ff:
        case 0x32fc1a8f:
        case 0x32fc1a90:
        case 0x32fc9418:
        case 0x32fc9619:
        case 0xc7fc9619:
        case 0xff050505:
        case 0xffead7be:
        case 0xfff7a540:
        case 0xfffa9c2a:
        case 0xfffc9619:
        case 0xfffcb654:
        case 0xfffd9c17:
        case 0xffffffff:
            return false;
        // make more transparent
        case 0xffbad6eb:
        case 0xffbae8eb:
        case 0xffeabed5:
        case 0xffead6bd:
            paint.color = 0x32000000 | (paint.color & 0xffffff);
            return true;
        // make it more white
        case 0xffa0a0a0:
            paint.color = 0xffc0c0c0;
            return true;
        }
    }

    switch (paint.color)
    {
    // scopes or other special things (do nothing)
    case 0x40ffffff:
    case 0xff004200:
    case 0xff2b281e:
    case 0xff2d2827:
    case 0xff303030:
    case 0xff362c23:
    case 0xff40352c:
    case 0xff5735d9:
    case 0xff5935d8:
    case 0xff5c26d9:
    case 0xff6140db:
    case 0xffa09400:
    case 0xffa19400:
    case 0xffa7a100:
    case 0xffa8a200:
    case 0xffaba102:
    case 0xff22a5e9:
    case 0xff6140da:
    case 0xff119ee6:
        return false;
    // pure black (convert to not quite pure white)
    case 0xff000000:
        paint.color = 0xffd0d0d0;
        return true;
    // all others (direct invert)
    default:
        paint.color = (paint.color & 0xff000000)
                    | (0xff0000 - (paint.color & 0xff0000))
                    | (0xff00 - (paint.color & 0xff00))
                    | (0xff - (paint.color & 0xff));
        return true;
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
            if (std::strncmp(filename + (filenamelen-filterlen), pathToFilterOut, filterlen) != 0)
                continue;

            puts("Removing CC-ND deadlock from file...");
            removeShape(handle, pathsToFilterOut[i].shapes[0]);
            removeShape(handle, pathsToFilterOut[i].shapes[1]);
            removeShape(handle, pathsToFilterOut[i].shapes[2]);
            removeShape(handle, pathsToFilterOut[i].shapes[3]);
            removeShape(handle, pathsToFilterOut[i].shapes[4]);
            removeShape(handle, pathsToFilterOut[i].shapes[5]);
            removeShape(handle, pathsToFilterOut[i].shapes[6]);
            removeShape(handle, pathsToFilterOut[i].shapes[7]);

            for (NSVGshape* shape = handle->shapes; shape != nullptr; shape = shape->next)
            {
                invertPaint(shape->fill);
                invertPaint(shape->stroke);
            }

            // replace white background
            handle->shapes->fill.type = NSVG_PAINT_COLOR;
            handle->shapes->fill.color = 0xff202020;
            handle->shapes->stroke.type = NSVG_PAINT_NONE;

            return handle;
        }

        for (size_t i = 0; i < sizeof(svgFilesToInvert)/sizeof(svgFilesToInvert[0]); ++i)
        {
            const char* const svgFileToInvert = svgFilesToInvert[i].filename;
            const size_t filenamelen = std::strlen(filename);
            const size_t filterlen = std::strlen(svgFileToInvert);

            if (filenamelen < filterlen)
                continue;
            if (std::strncmp(filename + (filenamelen-filterlen), svgFileToInvert, filterlen) != 0)
                continue;

            const char* const* const shapeIdsToIgnore = svgFilesToInvert[i].shapeIdsToIgnore;
            const int shapeNumberToIgnore = svgFilesToInvert[i].shapeNumberToIgnore;
            int shapeCounter = 0;

            for (NSVGshape* shape = handle->shapes; shape != nullptr; shape = shape->next, ++shapeCounter)
            {
                if (shapeNumberToIgnore == shapeCounter)
                    continue;

                bool ignore = false;
                for (size_t j = 0; j < 5 && shapeIdsToIgnore[j] != nullptr; ++j)
                {
                    if (std::strcmp(shape->id, shapeIdsToIgnore[j]) == 0)
                    {
                        ignore = true;
                        break;
                    }
                }
                if (ignore)
                    continue;

                if (invertPaint(shape->fill, svgFileToInvert))
                    invertPaint(shape->stroke, svgFileToInvert);
            }

            return handle;
        }

        // Special case for AmalgamatedHarmonics background color
        if (handle->shapes != nullptr && handle->shapes->fill.color == 0xff000000)
            if (std::strstr(filename, "/AmalgamatedHarmonics/") != nullptr)
                handle->shapes->fill.color = 0xff191919;

        return handle;
    }

    return nullptr;
}
