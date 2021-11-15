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
    const char* shapes[4];
} pathsToFilterOut[] = {
    {
        "/Core/AudioInterface.svg",
        {"path39377","path39381","path39383","path39379"}
    },
    {
        "/Core/AudioInterface2.svg",
        {"path18733","path18737","path18731","path18735"}
    },
    {
        "/Core/AudioInterface16.svg",
        {"path40283","path40287","path40289","path40285"}
    },
    {
        "/Core/CV-CC.svg",
        {"path12881","path12885","path12887","path12883"}
    },
    {
        "/Core/CV-Gate.svg",
        {"path13127","path13131","path13133","path13129"}
    },
    {
        "/Core/CV-MIDI.svg",
        {"path12747","path12751","path12753","path12749"}
    },
    {
        "/Core/MIDI-CC.svg",
        {"path9740","path9744","path9746","path9742"}
    },
    {
        "/Core/MIDI-CV.svg",
        {"path11803","path11807","path11809","path11805"}
    },
    {
        "/Core/MIDI-Gate.svg",
        {"path11634","path11638","path11640","path11636"}
    },
    {
        "/Core/MIDI-Map.svg",
        {"path21209","path21213","path21215","path21211"}
    },
    {
        "/Core/Notes.svg",
        {"path6935","path6939","path6941","path6937"}
    },
    {
        "/Fundamental/8vert.svg",
        {"path69642","path69646","path69640","path69644"}
    },
    {
        "/Fundamental/ADSR.svg",
        {"path33693","path33697","path33699","path33695"}
    },
    {
        "/Fundamental/Delay.svg",
        {"path25369","path25373","path25375","path25371"}
    },
    {
        "/Fundamental/LFO-1.svg",
        {"path35889","path35893","path35895","path35891"}
    },
    {
        "/Fundamental/LFO-2.svg",
        {"path36131","path36135","path36137","path36133"}
    },
    {
        "/Fundamental/Merge.svg",
        {"path29991","path29995","path29989","path29993"}
    },
    {
        "/Fundamental/MidSide.svg",
        {"path44181","path44185","path44179","path44183"}
    },
    {
        "/Fundamental/Mutes.svg",
        {"path21613","path21617","path21611","path21615"}
    },
    {
        "/Fundamental/Noise.svg",
        {"path105594","path105598","path105592","path105596"}
    },
    {
        "/Fundamental/Octave.svg",
        {"path38471","path38475","path38469","path38473"}
    },
    {
        "/Fundamental/Pulses.svg",
        {"path46241","path46245","path46239","path46243"}
    },
    {
        "/Fundamental/Quantizer.svg",
        {"path38549","path38553","path38547","path38551"}
    },
    {
        "/Fundamental/Random.svg",
        {"path89732","path89736","path89730","path89734"}
    },
    {
        "/Fundamental/SEQ3.svg",
        {"path35687","path35691","path35693","path35689"}
    },
    {
        "/Fundamental/Scope.svg",
        {"path33887","path33891","path33893","path33889"}
    },
    {
        "/Fundamental/Split.svg",
        {"path29999","path30003","path29997","path30001"}
    },
    {
        "/Fundamental/Sum.svg",
        {"path10913","path10917","path10911","path10915"}
    },
    {
        "/Fundamental/Unity.svg",
        {"path21219","path21223","path21217","path21221"}
    },
    {
        "/Fundamental/VCF.svg",
        {"path25239","path25243","path25245","path25241"}
    },
    {
        "/Fundamental/VCMixer.svg",
        {"path125839","path125843","path125845","path125841"}
    },
    {
        "/Fundamental/VCO-1.svg",
        {"path33533","path33537","path33539","path33535"}
    },
    {
        "/Fundamental/VCO-2.svg",
        {"path37557","path37561","path37563","path37559"}
    },
    {
        "/Fundamental/Viz.svg",
        {"path41769","path41773","path41767","path41771"}
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
    { "/Bidoo/BAR.svg", {"rect833"}, -1 },
    { "/Bidoo/BISTROT.svg", {}, -1 },
    { "/Bidoo/BORDL.svg", {"rect959-3-0-7-5","rect959-3-0-7","rect959-3-0","rect959-3"}, -1 },
    { "/Bidoo/CANARD.svg", {"rect959-3-7"}, -1 },
    { "/Bidoo/CHUTE.svg", {}, -1 },
    { "/Bidoo/DFUZE.svg", {}, -1 },
    { "/Bidoo/DIKTAT.svg", {"rect843","rect843-0","rect843-0-8"}, -1 },
    { "/Bidoo/DTROY.svg", {"rect959-3"}, -1 },
    { "/Bidoo/DUKE.svg", {}, -1 },
    { "/Bidoo/EMILE.svg", {}, -1 },
    { "/Bidoo/FLAME.svg", {}, -1 },
    { "/Bidoo/FORK.svg", {}, -1 },
    { "/Bidoo/FREIN.svg", {}, -1 },
    { "/Bidoo/HCTIP.svg", {}, -1 },
    { "/Bidoo/HUITRE.svg", {}, -1 },
    { "/Bidoo/LATE.svg", {}, -1 },
    { "/Bidoo/LIMBO.svg", {}, -1 },
    { "/Bidoo/LIMONADE.svg", {"rect839","rect839-6"}, -1 },
    { "/Bidoo/LOURDE.svg", {}, -1 },
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
    { "/Fundamental/SequentialSwitch1.svg", {}, -1 },
    { "/Fundamental/SequentialSwitch2.svg", {}, -1 },
    { "/Fundamental/VCA-1.svg", {}, -1 },
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

static void removeShape(NSVGimage* const handle, const char* const id)
{
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
        break;
    }
}

static bool invertPaint(NSVGpaint& paint, const char* const svgFileToInvert = nullptr)
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

            for (NSVGshape* shape = handle->shapes; shape != nullptr; shape = shape->next)
            {
                invertPaint(shape->fill);
                invertPaint(shape->stroke);
            }

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
