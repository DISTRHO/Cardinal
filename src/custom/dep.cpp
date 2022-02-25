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

#include "nanovg.h"

// fix bogaudio build, another missing symbol
#ifndef NDEBUG
namespace bogaudio {
struct FollowerBase {
    static float efGainMaxDecibelsDebug;
};
float FollowerBase::efGainMaxDecibelsDebug = 12.0f;
}
#endif

// Special nvgRGB for blank panels
extern "C" {
NVGcolor nvgRGBblank(unsigned char, unsigned char, unsigned char)
{
    return nvgRGB(0x20, 0x20, 0x20);
}
}

// Compile those nice implementation-in-header little libraries
#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#undef nsvgParseFromFile
#include <nanosvg.h>

// Custom Cardinal filtering
static const struct {
    const char* const filename;
    const char* shapeIdsToIgnore[5];
    const int shapeNumberToIgnore;
} svgFilesToInvert[] = {
    // MIT
    { "/21kHz/res/Panels/D_Inf.svg", {}, -1 },
    { "/21kHz/res/Panels/PalmLoop.svg", {}, -1 },
    { "/21kHz/res/Panels/TachyonEntangler.svg", {}, -1 },
    // Custom, runtime dark mode used with permission
    { "/AudibleInstruments/res/Blinds.svg", {}, -1 },
    { "/AudibleInstruments/res/Braids.svg", {}, -1 },
    { "/AudibleInstruments/res/Branches.svg", {}, -1 },
    { "/AudibleInstruments/res/Clouds.svg", {}, -1 },
    { "/AudibleInstruments/res/Elements.svg", {}, -1 },
    { "/AudibleInstruments/res/Frames.svg", {}, -1 },
    { "/AudibleInstruments/res/Kinks.svg", {}, -1 },
    { "/AudibleInstruments/res/Links.svg", {}, -1 },
    { "/AudibleInstruments/res/Marbles.svg", {}, -1 },
    { "/AudibleInstruments/res/Peaks.svg", {}, -1 },
    { "/AudibleInstruments/res/Plaits.svg", {}, -1 },
    { "/AudibleInstruments/res/Rings.svg", {}, -1 },
    { "/AudibleInstruments/res/Ripples.svg", {}, -1 },
    { "/AudibleInstruments/res/Shades.svg", {}, -1 },
    { "/AudibleInstruments/res/Sheep.svg", {}, -1 },
    { "/AudibleInstruments/res/Shelves.svg", {}, -1 },
    { "/AudibleInstruments/res/Stages.svg", {}, -1 },
    { "/AudibleInstruments/res/Streams.svg", {}, -1 },
    { "/AudibleInstruments/res/Tides.svg", {}, -1 },
    { "/AudibleInstruments/res/Tides2.svg", {}, -1 },
    { "/AudibleInstruments/res/Veils.svg", {}, -1 },
    { "/AudibleInstruments/res/Warps.svg", {}, -1 },
    // CC-BY-NC-ND-4.0, runtime dark mode used with permission
    { "/Bidoo/res/ACNE.svg", {}, -1 },
    { "/Bidoo/res/ANTN.svg", {}, -1 },
    { "/Bidoo/res/BAFIS.svg", {}, -1 },
    { "/Bidoo/res/BANCAU.svg", {}, -1 },
    { "/Bidoo/res/BAR.svg", {"rect833"}, -1 },
    { "/Bidoo/res/BISTROT.svg", {}, -1 },
    { "/Bidoo/res/BORDL.svg", {"rect959-3-0-7-5","rect959-3-0-7","rect959-3-0","rect959-3"}, -1 },
    { "/Bidoo/res/CANARD.svg", {"rect959-3-7"}, -1 },
    { "/Bidoo/res/CHUTE.svg", {}, -1 },
    { "/Bidoo/res/DFUZE.svg", {}, -1 },
    { "/Bidoo/res/DIKTAT.svg", {"rect843","rect843-0","rect843-0-8"}, -1 },
    { "/Bidoo/res/DILEMO.svg", {}, -1 },
    { "/Bidoo/res/DTROY.svg", {"rect959-3"}, -1 },
    { "/Bidoo/res/DUKE.svg", {}, -1 },
    { "/Bidoo/res/EDSAROS.svg", {"rect959-3-7","rect959-3-7-8","rect959-3-7-8-1","rect959-3-7-8-1-4"}, -1 },
    { "/Bidoo/res/EMILE.svg", {}, -1 },
    { "/Bidoo/res/FLAME.svg", {}, -1 },
    { "/Bidoo/res/FORK.svg", {}, -1 },
    { "/Bidoo/res/FREIN.svg", {}, -1 },
    { "/Bidoo/res/HCTIP.svg", {}, -1 },
    { "/Bidoo/res/HUITRE.svg", {}, -1 },
    { "/Bidoo/res/LAMBDA.svg", {}, -1 },
    { "/Bidoo/res/LATE.svg", {}, -1 },
    { "/Bidoo/res/LIMBO.svg", {}, -1 },
    { "/Bidoo/res/LIMONADE.svg", {"rect839","rect839-6"}, -1 },
    { "/Bidoo/res/LOURDE.svg", {"rect847","rect847-7","rect847-5","rect847-3"}, -1 },
    { "/Bidoo/res/MAGMA.svg", {}, -1 },
    { "/Bidoo/res/MINIBAR.svg", {"rect833"}, -1 },
    { "/Bidoo/res/MOIRE.svg", {"rect843","rect843-7"}, -1 },
    { "/Bidoo/res/MS.svg", {}, -1 },
    { "/Bidoo/res/MU.svg", {"rect864"}, -1 },
    { "/Bidoo/res/OAI.svg", {}, -1 },
    { "/Bidoo/res/OUAIVE.svg", {"rect959-3-7"}, -1 },
    { "/Bidoo/res/PERCO.svg", {}, -1 },
    { "/Bidoo/res/PILOT.svg", {"rect843-6-4-5","rect843","rect843-4","rect843-6-4","rect843-6-7"}, -1 },
    { "/Bidoo/res/POUPRE.svg", {}, -1 },
    { "/Bidoo/res/RABBIT.svg", {}, -1 },
    { "/Bidoo/res/REI.svg", {}, -1 },
    { "/Bidoo/res/SIGMA.svg", {}, -1 },
    { "/Bidoo/res/SPORE.svg", {}, -1 },
    { "/Bidoo/res/TIARE.svg", {}, -1 },
    { "/Bidoo/res/TOCANTE.svg", {"rect843"}, -1 },
    { "/Bidoo/res/VOID.svg", {}, -1 },
    { "/Bidoo/res/ZINC.svg", {}, -1 },
    { "/Bidoo/res/ZOUMAI.svg", {}, -1 },
    // BSD-3-Clause
    { "/cf/res/ALGEBRA.svg", {}, -1 },
    { "/cf/res/BUFFER.svg", {}, -1 },
    { "/cf/res/CHOKE.svg", {}, -1 },
    { "/cf/res/CUBE.svg", {}, -1 },
    { "/cf/res/CUTS.svg", {}, -1 },
    { "/cf/res/DISTO.svg", {}, -1 },
    { "/cf/res/EACH.svg", {}, -1 },
    { "/cf/res/FOUR.svg", {}, -1 },
    { "/cf/res/FUNKTION.svg", {}, -1 },
    { "/cf/res/L3DS3Q.svg", {}, 3 },
    { "/cf/res/LABEL.svg", {}, -1 },
    { "/cf/res/LEDS.svg", {}, -1 },
    { "/cf/res/LEDSEQ.svg", {}, 3 },
    { "/cf/res/MASTER.svg", {}, -1 },
    { "/cf/res/METRO.svg", {}, -1 },
    { "/cf/res/MONO.svg", {}, -1 },
    { "/cf/res/PATCH.svg", {}, -1 },
    { "/cf/res/PEAK.svg", {}, -1 },
    { "/cf/res/PLAY.svg", {}, -1 },
    { "/cf/res/PLAYER.svg", {}, -1 },
    { "/cf/res/SLIDERSEQ.svg", {}, -1 },
    { "/cf/res/STEPS.svg", {}, -1 },
    { "/cf/res/STEREO.svg", {}, -1 },
    { "/cf/res/SUB.svg", {}, -1 },
    { "/cf/res/trSEQ.svg", {}, -1 },
    { "/cf/res/VARIABLE.svg", {}, -1 },
    // CC0-1.0
    { "/DrumKit/res/Baronial.svg", {}, -1 },
    { "/DrumKit/res/BD9.svg", {}, -1 },
    { "/DrumKit/res/ClosedHH.svg", {}, -1 },
    { "/DrumKit/res/CR78.svg", {}, -1 },
    { "/DrumKit/res/DMX.svg", {}, -1 },
    { "/DrumKit/res/Gnome.svg", {}, -1 },
    { "/DrumKit/res/Marionette.svg", {}, -1 },
    { "/DrumKit/res/OpenHH.svg", {}, -1 },
    { "/DrumKit/res/SBD.svg", {}, -1 },
    { "/DrumKit/res/Sequencer.svg", {}, -1 },
    { "/DrumKit/res/Snare.svg", {}, -1 },
    { "/DrumKit/res/Tomi.svg", {}, -1 },
    // Custom, runtime dark mode used with permission
    { "/ESeries/res/E340.svg", {}, -1 },
    // CC0-1.0
    { "/HetrickCV/res/1OpChaos.svg", {}, -1},
    { "/HetrickCV/res/2OpChaos.svg", {}, -1},
    { "/HetrickCV/res/2To4.svg", {}, -1},
    { "/HetrickCV/res/3OpChaos.svg", {}, -1},
    { "/HetrickCV/res/ASR.svg", {}, -1},
    { "/HetrickCV/res/AToD.svg", {}, -1},
    { "/HetrickCV/res/BinaryGate.svg", {}, -1},
    { "/HetrickCV/res/BinaryNoise.svg", {}, -1},
    { "/HetrickCV/res/Bitshift.svg", {}, -1},
    { "/HetrickCV/res/Boolean3.svg", {}, -1},
    { "/HetrickCV/res/ChaoticAttractors.svg", {}, -1},
    { "/HetrickCV/res/ClockedNoise.svg", {}, -1},
    { "/HetrickCV/res/Comparator.svg", {}, -1},
    { "/HetrickCV/res/Contrast.svg", {}, -1},
    { "/HetrickCV/res/Crackle.svg", {}, -1},
    { "/HetrickCV/res/DataCompander.svg", {}, -1},
    { "/HetrickCV/res/Delta.svg", {}, -1},
    { "/HetrickCV/res/DToA.svg", {}, -1},
    { "/HetrickCV/res/Dust.svg", {}, -1},
    { "/HetrickCV/res/Exponent.svg", {}, -1},
    { "/HetrickCV/res/FBSineChaos.svg", {}, -1},
    { "/HetrickCV/res/FlipFlop.svg", {}, -1},
    { "/HetrickCV/res/FlipPan.svg", {}, -1},
    { "/HetrickCV/res/GateJunction.svg", {}, -1},
    { "/HetrickCV/res/Gingerbread.svg", {}, -1},
    { "/HetrickCV/res/LogicCombiner.svg", {}, -1},
    { "/HetrickCV/res/LogicInverter.svg", {}, -1},
    { "/HetrickCV/res/MidSide.svg", {}, -1},
    { "/HetrickCV/res/MinMax.svg", {}, -1},
    { "/HetrickCV/res/RandomGates.svg", {}, -1},
    { "/HetrickCV/res/Rotator.svg", {}, -1},
    { "/HetrickCV/res/Rungler.svg", {}, -1},
    { "/HetrickCV/res/Scanner.svg", {}, -1},
    { "/HetrickCV/res/TrigShaper.svg", {}, -1},
    { "/HetrickCV/res/Waveshape.svg", {}, -1},
    { "/HetrickCV/res/XYToPolar.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel1.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel2.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel3.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel5.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel6.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel7.svg", {}, -1},
    { "/HetrickCV/res/Blanks/BlankPanel8.svg", {}, -1},
    // BSD-3-Clause
    { "/JW-Modules/res/Add5.svg", {}, -1 },
    { "/JW-Modules/res/BlankPanel1hp.svg", {}, -1 },
    { "/JW-Modules/res/BlankPanelLarge.svg", {}, -1 },
    { "/JW-Modules/res/BlankPanelMedium.svg", {}, -1 },
    { "/JW-Modules/res/BlankPanelSmall.svg", {}, -1 },
    { "/JW-Modules/res/BouncyBalls.svg", {}, -1 },
    { "/JW-Modules/res/D1v1de.svg", {}, -1 },
    { "/JW-Modules/res/DivSeq.svg", {}, -1 },
    { "/JW-Modules/res/EightSeq.svg", {}, -1 },
    { "/JW-Modules/res/GridSeq.svg", {}, -1 },
    { "/JW-Modules/res/MinMax.svg", {"path38411"}, -1 },
    { "/JW-Modules/res/NoteSeq.svg", {}, -1 },
    { "/JW-Modules/res/NoteSeq16.svg", {}, -1 },
    { "/JW-Modules/res/NoteSeqFu.svg", {}, -1 },
    { "/JW-Modules/res/OnePattern.svg", {}, -1 },
    { "/JW-Modules/res/Patterns.svg", {}, -1 },
    { "/JW-Modules/res/Pres1t.svg", {}, -1 },
    { "/JW-Modules/res/PT.svg", {}, -1 },
    { "/JW-Modules/res/Str1ker.svg", {"rect2094","rect995","rect169"}, -1 },
    { "/JW-Modules/res/Trigs.svg", {}, -1 },
    { "/JW-Modules/res/WavHeadPanel.svg", {}, -1 },
    { "/JW-Modules/res/XYPad.svg", {}, -1 },
    // GPL3.0-or-later
    { "/LilacLoop/res/Looper.svg", {}, -1 },
    // EUPL-1.2
    { "/LittleUtils/res/Bias_Semitone.svg", {}, -1 },
    { "/LittleUtils/res/ButtonModule.svg", {}, -1 },
    { "/LittleUtils/res/MulDiv.svg", {}, -1 },
    { "/LittleUtils/res/PulseGenerator.svg", {}, -1 },
    { "/LittleUtils/res/TeleportIn.svg", {}, -1 },
    { "/LittleUtils/res/TeleportOut.svg", {}, -1 },
    // GPL-3.0-or-later
    { "/kocmoc/res/DDLY.svg", {}, -1 },
    { "/kocmoc/res/LADR.svg", {}, -1 },
    { "/kocmoc/res/MUL.svg", {}, -1 },
    { "/kocmoc/res/OP.svg", {}, -1 },
    { "/kocmoc/res/PHASR.svg", {}, -1 },
    { "/kocmoc/res/SKF.svg", {}, -1 },
    { "/kocmoc/res/SVF.svg", {}, -1 },
    { "/kocmoc/res/TRG.svg", {}, -1 },
};

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

    // Special case for Lilac
    if (svgFileToInvert != nullptr && std::strncmp(svgFileToInvert, "/LilacLoop/", 11) == 0)
    {
        switch (paint.color)
        {
        // main bg (custom)
        case 0xffd5d5da:
            paint.color = 0xff242228;
            return true;
        // main color (do nothing)
        case 0xffbfb7d7:
            return false;
        // screws (hide)
        case 0xffc8c8cf:
        case 0xffbcbcbc:
        case 0xffb1b1bb:
        case 0xffacacac:
        case 0xff898991:
        case 0xff727272:
            paint.color = 0x00000000;
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
    case 0xff2432ed:
    case 0xff0095fe:
    case 0xff4d9a4d:
    case 0xff4d4d9a:
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
