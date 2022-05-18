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

/**
 * This file is copied and adapted from Sassy Audio Spreadsheet (sassy_scope.cpp)
 * Copyright (c) 2022 Jari Komppa.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "sassy.hpp"

#define POW_2_3_4TH 1.6817928305074290860622509524664297900800685247135690216264521719

static double catmullrom(double t, double p0, double p1, double p2, double p3)
{
    return 0.5 * (
        (2 * p1) +
        (-p0 + p2) * t +
        (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
        (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t
        );
}

static const char* gNotestr[128] =
{
    "C0-1","C#-1","D-1","D#-1","E-1","F-1","F#-1","G-1","G#-1","A-1","A#-1","B-1",
    "C0","C#0","D0","D#0","E0","F0","F#0","G0","G#0","A0","A#0","B0",
    "C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1",
    "C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2",
    "C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3",
    "C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4",
    "C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5",
    "C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6",
    "C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7",
    "C8","C#8","D8","D#8","E8","F8","F#8","G8","G#8","A8","A#8","B8",
    "C9","C#9","D9","D#9","E9","F9","F#9","G9"
};

static const float timescalesteps[5] =
{
    0.0001f,
    0.001f,
    0.01f,
    0.1f,
    1.0f,
};

static const char* timescaletext[5] =
{
    "0.1ms",
    "1ms",
    "10ms",
    "100ms",
    "1000ms"
};

static const float scalesteps[9] =
{
    1.0f / 32.0f,
    1.0f / 16.0f,
    1.0f / 10.0f,
    1.0f / 8.0f,
    1.0f / 5.0f,
    1.0f / 4.0f,
    1.0f / 2.0f,
    1.0f,
    2.0f,
};

static const char* scaletexts[9] = {
    "x1/32",
    "x1/16",
    "x1/10",
    "x1/8",
    "x1/5",
    "x1/4",
    "x1/2",
    "x1",
    "x2",
};

static constexpr const int grid_size = 340;
static constexpr const int grid_half_size = grid_size / 2 + 10;
static constexpr const int grid_quarter_size = static_cast<int>(grid_half_size / 2);
static constexpr const int grid_1_8_size = grid_quarter_size / 2;
static constexpr const int grid_3_8_size = grid_quarter_size + grid_quarter_size / 2;

static void scope_grid(const float uiScale)
{
    ImVec2 p = ImGui::GetItemRectMin();

    // zero
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2) * uiScale), 0xff000000, 3.0f);
    // 1.0
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2 - grid_quarter_size) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2 - grid_quarter_size) * uiScale), 0xff000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2 + grid_quarter_size) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2 + grid_quarter_size) * uiScale), 0xff000000);
    // 0.5
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2 - grid_1_8_size) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2 - grid_1_8_size) * uiScale), 0x3f000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2 + grid_1_8_size) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2 + grid_1_8_size) * uiScale), 0x3f000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2 - grid_3_8_size) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2 - grid_3_8_size) * uiScale), 0x3f000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + (grid_size / 2 + grid_3_8_size) * uiScale), ImVec2(p.x + (grid_size) * uiScale, p.y + (grid_size / 2 + grid_3_8_size) * uiScale), 0x3f000000);

    // zero
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2) * uiScale, p.y + (grid_size) * uiScale), 0xff000000, 3.0f);
    // 1.0
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2 - grid_quarter_size) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2 - grid_quarter_size) * uiScale, p.y + (grid_size) * uiScale), 0xff000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2 + grid_quarter_size) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2 + grid_quarter_size) * uiScale, p.y + (grid_size) * uiScale), 0xff000000);
    // 0.5
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2 - grid_1_8_size) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2 - grid_1_8_size) * uiScale, p.y + (grid_size) * uiScale), 0x3f000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2 + grid_1_8_size) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2 + grid_1_8_size) * uiScale, p.y + (grid_size) * uiScale), 0x3f000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2 - grid_3_8_size) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2 - grid_3_8_size) * uiScale, p.y + (grid_size) * uiScale), 0x3f000000);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (grid_size / 2 + grid_3_8_size) * uiScale, p.y * uiScale), ImVec2(p.x + (grid_size / 2 + grid_3_8_size) * uiScale, p.y + (grid_size) * uiScale), 0x3f000000);
}


static int scope_sync(ScopeData* gScope, int index)
{
    const float gSamplerate = gScope->mSampleRate;
    int samples = (int)(gSamplerate * gScope->mTimeScale);
    int cycle = gSamplerate * 10;
    int ofs = samples;

    if (gScope->mMode == 0)
    {
        // calculate sync
        if (gScope->mSyncMode == 0)
        {
            float* graphdata = gScope->mCh[gScope->mSyncChannel].mData;
            int over = ofs;
            while (over < (cycle - ofs) && graphdata[(index - over + cycle) % cycle] < 0) over++;
            int under = over;
            while (under < (cycle - ofs) && graphdata[(index - under + cycle) % cycle] > 0) under++;
            ofs = under;
        }
        else
            if (gScope->mSyncMode == 1)
            {
                float* graphdata = gScope->mCh[gScope->mSyncChannel].mData;
                int under = ofs;
                while (under < (cycle - ofs) && graphdata[(index - under + cycle) % cycle] > 0) under++;
                int over = under;
                while (over < (cycle - ofs) && graphdata[(index - over + cycle) % cycle] < 0) over++;
                ofs = over;
            }
        // default: ofs = samples
    }
    else
    {
        // pause mode, scroll bar is active
        ofs = -(int)(gScope->mScroll * gSamplerate);
        if (ofs < samples)
            ofs = samples;
        if (ofs > gSamplerate * 10 - samples)
            ofs = gSamplerate * 10 - samples;
    }
    gScope->mScroll = -((float)ofs / gSamplerate);

    return ofs;
}

#if 0
static void scope_plot(ScopeData* gScope, const float uiScale, int index)
{
    ImVec2 p = ImGui::GetItemRectMin();
    const float gSamplerate = gScope->mSampleRate;
    int cycle = gSamplerate * 10;
    /*
    Okay, max scale is 1 second, so..
    */
    int samples = (int)(gSamplerate * gScope->mTimeScale);

    scope_grid(uiScale);

    int ofs = scope_sync(gScope, index);

    if (gScope->mDisplay == 2)
    {
        for (int i = 0; i < 16384; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (gScope->mCh[j].mEnabled)
                {
                    float* graphdata = gScope->mCh[j].mData;
                    float y = graphdata[(index - ofs + i * samples / 16384 + cycle) % cycle];
                    float x = graphdata[(index - ofs + i * samples / 16384 + cycle - 1) % cycle];
                    x = x * gScope->mCh[j].mScale;
                    y = y * gScope->mCh[j].mScale - gScope->mCh[j].mOffset;
                    ImGui::GetWindowDrawList()->AddCircleFilled(
                        ImVec2(p.x + (grid_size / 2 + x * grid_quarter_size) * uiScale, p.y + (grid_size / 2 + y * grid_quarter_size) * uiScale),
                        1,
                        (gScope->colors[j] & 0xffffff) | 0x3f000000);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < 32768; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (gScope->mCh[j*2].mEnabled)
                {
                    float* graphdata = gScope->mCh[j * 2].mData;
                    float x = graphdata[(index - ofs + i * samples / 32768 + cycle) % cycle];
                    graphdata = gScope->mCh[j * 2 + 1].mData;
                    float y = graphdata[(index - ofs + i * samples / 32768 + cycle) % cycle];
                    x = x * gScope->mCh[j * 2].mScale - gScope->mCh[j * 2].mOffset;
                    y = y * gScope->mCh[j * 2 + 1].mScale - gScope->mCh[j * 2 + 1].mOffset;
                    ImGui::GetWindowDrawList()->AddCircleFilled(
                        ImVec2(p.x + (grid_size / 2 + x * grid_quarter_size) * uiScale, p.y + (grid_size / 2 + y * grid_quarter_size) * uiScale),
                        1,
                        (gScope->colors[j*2] & 0xffffff) | 0x3f000000);
                }
            }
        }
    }

}
#endif

static void scope_time(ScopeData* gScope, const float uiScale, int index)
{
    ImVec2 p = ImGui::GetItemRectMin();
    const float gSamplerate = gScope->mSampleRate;
    int cycle = gSamplerate * 10;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    /*
    Okay, max scale is 1 second, so..
    */
    int samples = (int)(gSamplerate * gScope->mTimeScale);

    scope_grid(uiScale);

    int ofs = scope_sync(gScope, index);

    if (samples > grid_size)
    {
        for (int j = 0; j < 4; j++)
        {
            if (gScope->mCh[j].mEnabled)
            {
                float* graphdata = gScope->mCh[j].mData;
                ImVec2 vert[grid_size];
                for (int i = 0; i < grid_size; i++)
                {
                    float v0 = -graphdata[(index - ofs + i * samples / grid_size + cycle) % cycle];
                    float v1 = -graphdata[(index - ofs + (i + 1) * samples / grid_size + cycle) % cycle];
                    v0 = v0 * gScope->mCh[j].mScale - gScope->mCh[j].mOffset;
                    v1 = v1 * gScope->mCh[j].mScale - gScope->mCh[j].mOffset;
                    vert[i].x = p.x + i * uiScale;
                    vert[i].y = p.y + (grid_size / 2 + v0 * grid_quarter_size) * uiScale;
                }
                float v0 = p.y + (grid_size / 2 + (-gScope->mCh[j].mOffset) * grid_quarter_size) * uiScale;
                dl->Flags = 0;
                for (int i = 0; i < grid_size-1; i++)
                {
                    ImVec2 quad[4];
                    quad[0] = ImVec2(vert[i].x, v0);
                    quad[1] = ImVec2(vert[i].x, vert[i].y);
                    quad[2] = ImVec2(vert[i + 1].x, vert[i + 1].y);
                    quad[3] = ImVec2(vert[i + 1].x, v0);
                    dl->AddConvexPolyFilled(quad, 4, (gScope->colors[j] & 0xffffff) | 0x3f000000 );

                }

                if (gScope->mTimeScale < 0.1)
                {
                    dl->Flags = ImDrawListFlags_AntiAliasedLines;
                    dl->AddPolyline(vert, grid_size, gScope->colors[j], false, 2 * uiScale);
                }
                else
                {
                    dl->Flags = ImDrawListFlags_AntiAliasedLines;
                    dl->AddPolyline(vert, grid_size, gScope->colors[j], false, 1);

                }
            }
        }
    }
    else
    {
        // less than 1 sample per pixel
        for (int j = 0; j < 4; j++)
        {
            if (gScope->mCh[j].mEnabled)
            {
                float* graphdata = gScope->mCh[j].mData;
                for (int i = 0; i < samples; i++)
                {
                    float v0 = -graphdata[(index - ofs + i + cycle) % cycle];
                    float v1 = 0;
                    v0 = v0 * gScope->mCh[j].mScale - gScope->mCh[j].mOffset;
                    v1 = v1 * gScope->mCh[j].mScale - gScope->mCh[j].mOffset;
                    float x0 = p.x + (i * grid_size / samples) * uiScale;
                    ImGui::GetWindowDrawList()->AddCircleFilled(
                        ImVec2(x0, p.y + (grid_size / 2 + v0 * grid_quarter_size) * uiScale),
                        4 * uiScale,
                        gScope->colors[j]);
                    ImGui::GetWindowDrawList()->AddLine(
                        ImVec2(x0, p.y + (grid_size / 2 + v0 * grid_quarter_size) * uiScale),
                        ImVec2(x0, p.y + (grid_size / 2 + v1 * grid_quarter_size) * uiScale),
                        gScope->colors[j]);
                }
            }
        }
    }

    ImGui::GetWindowDrawList()->AddText(p, 0xffc0c0c0, timescaletext[gScope->mTimeScaleSlider + 2]);
    ImGui::GetWindowDrawList()->AddText(ImVec2(p.x, p.y + (grid_size / 2 - grid_quarter_size - 7) * uiScale), 0xffc0c0c0, "+1");
    ImGui::GetWindowDrawList()->AddText(ImVec2(p.x, p.y + (grid_size / 2 + grid_quarter_size - 7) * uiScale), 0xffc0c0c0, "-1");

    ImVec2 mp = ImGui::GetMousePos();
    mp.x -= p.x;
    mp.y -= p.y;
    if (mp.x > 0 && mp.x < grid_size * uiScale &&
        mp.y > 0 && mp.y < grid_size * uiScale)
    {
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(p.x + mp.x, p.y),
            ImVec2(p.x + mp.x, p.y + grid_size * uiScale),
            0xff00ff00, 1 * uiScale);
        if (gScope->mCh[0].mEnabled || gScope->mCh[1].mEnabled || gScope->mCh[2].mEnabled || gScope->mCh[3].mEnabled)
        {
            ImGui::BeginTooltip();
            if (gScope->mCh[0].mEnabled) ImGui::Text("Ch 0: %3.3f", gScope->mCh[0].mData[(index - ofs + ((int)mp.x / (int)uiScale) * samples / grid_size + cycle) % cycle]);
            if (gScope->mCh[1].mEnabled) ImGui::Text("Ch 1: %3.3f", gScope->mCh[1].mData[(index - ofs + ((int)mp.x / (int)uiScale) * samples / grid_size + cycle) % cycle]);
            if (gScope->mCh[2].mEnabled) ImGui::Text("Ch 2: %3.3f", gScope->mCh[2].mData[(index - ofs + ((int)mp.x / (int)uiScale) * samples / grid_size + cycle) % cycle]);
            if (gScope->mCh[3].mEnabled) ImGui::Text("Ch 3: %3.3f", gScope->mCh[3].mData[(index - ofs + ((int)mp.x / (int)uiScale) * samples / grid_size + cycle) % cycle]);
            ImGui::EndTooltip();
        }
    }
}


static void vertline(const float uiScale, const float x, const float w)
{
    ImVec2 p = ImGui::GetItemRectMin();
    ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + x * uiScale, p.y * uiScale), ImVec2(p.x + x * uiScale, p.y + (grid_size) * uiScale), 0xff000000, w);
}

static void scope_freq(ScopeData* gScope, const float uiScale, int index)
{
    ImVec2 p = ImGui::GetItemRectMin();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float gSamplerate = gScope->mSampleRate;
    int cycle = gSamplerate * 10;
    /*
    Okay, max scale is 1 second, so..
    */
    int samples = (int)(gSamplerate * gScope->mTimeScale);

    // what's the biggest PoT < samples?
    // 192000 takes 18 bits to encode.
    // Fill 32 bits:
    int pot = samples | (samples >> 16);
    pot = pot | (pot >> 8);
    pot = pot | (pot >> 4);
    pot = pot | (pot >> 2);
    pot = pot | (pot >> 1);
    // Shift down and add one to round it up
    pot = (pot >> 1) + 1;

    if (pot < 16) pot = 16;
    if (pot > 65536) pot = 65536;

    gScope->mPot = pot;

    ffft::FFTReal<float>* fft = NULL;
    switch (pot)
    {
    case 16: fft = gScope->fft.obj16; break;
    case 32: fft = gScope->fft.obj32; break;
    case 64: fft = gScope->fft.obj64; break;
    case 128: fft = gScope->fft.obj128; break;
    case 256: fft = gScope->fft.obj256; break;
    case 512: fft = gScope->fft.obj512; break;
    case 1024: fft = gScope->fft.obj1024; break;
    case 2048: fft = gScope->fft.obj2048; break;
    case 4096: fft = gScope->fft.obj4096; break;
    case 8192: fft = gScope->fft.obj8192; break;
    case 16384: fft = gScope->fft.obj16384; break;
    case 32768: fft = gScope->fft.obj32768; break;
    case 65536: fft = gScope->fft.obj65536; break;
    }
    if (!fft) return;

    int average = gScope->fft.average;
    int ofs = scope_sync(gScope, index);
    int size = grid_size - 1;
    float sizef = size;
    float freqbin = gSamplerate / (float)(pot / 2);
    float freqbins[size];
    float zoom = 1.0f / (1 << gScope->mFFTZoom);

    for (int i = 0; i < 10; i++)
    {     
        vertline(uiScale, sqrt(100 / freqbin * i / (pot / 4)) / zoom * sizef, 1);
        vertline(uiScale, sqrt(1000 / freqbin * i / (pot / 4)) / zoom * sizef, 1);
        vertline(uiScale, sqrt(10000 / freqbin * i / (pot / 4)) / zoom * sizef, 1);
    }

    for (int j = 0; j < 4; j++)
    {
        if (gScope->mCh[j].mEnabled)
        {


            memset(gScope->ffta, 0, sizeof(float) * 65536 * 2);
            for (int k = 0; k < average; k++)
            {
                float* graphdata = gScope->mCh[j].mData;

                for (int i = 0; i < pot; i++)
                {
                    gScope->fft1[i * 2] = graphdata[(index - ofs + i + cycle - k) % cycle];
                    gScope->fft1[i * 2 + 1] = 0;
                }

                fft->do_fft(gScope->fft2, gScope->fft1);

                for (int i = 0; i < pot / 4; i++)
                    gScope->ffta[i] += (1.0f / average) * sqrt(gScope->fft2[i * 2 + 0] * gScope->fft2[i * 2 + 0] + gScope->fft2[i * 2 + 1] * gScope->fft2[i * 2 + 1]);
            }

            ImVec2 vert[size];

            for (int i = 0; i < size; i++)
            {
                float ppos = powf(zoom * i / sizef, 2.0f) * pot / 4;
                freqbins[i] = ppos * freqbin;
                
                float f = ppos - (int)ppos;
                float a = i ? gScope->ffta[(int)ppos - 1] : 0;
                float b = gScope->ffta[(int)ppos];
                float c = i < size ? gScope->ffta[(int)ppos + 1] : 0;
                float d = i < (size-1) ? gScope->ffta[(int)ppos + 2] : 0;

                float v0 = (float)catmullrom(f, a, b, c, d);
                
                v0 = v0 * gScope->mCh[j].mScale + gScope->mCh[j].mOffset * 50;
                vert[i] = ImVec2(p.x + i * uiScale, 
                                 p.y + (sizef - v0 * 4) * uiScale);
            }
            float v0 = p.y + (size - gScope->mCh[j].mOffset * 50 * 4) * uiScale;
            dl->Flags = 0;
            for (int i = 0; i < size-1; i++)
            {
                ImVec2 quad[4];
                quad[0] = ImVec2(vert[i].x, v0);
                quad[1] = ImVec2(vert[i].x, vert[i].y);
                quad[2] = ImVec2(vert[i + 1].x, vert[i + 1].y);
                quad[3] = ImVec2(vert[i + 1].x, v0);
                dl->AddConvexPolyFilled(quad, 4, (gScope->colors[j] & 0xffffff) | 0x3f000000);

            }

            dl->Flags = ImDrawListFlags_AntiAliasedLines;
            dl->AddPolyline(vert, size, gScope->colors[j], false, 1);

        }
    }

    if (!ImGui::IsPopupOpen("Freq Context",ImGuiPopupFlags_AnyPopupId))
    if (gScope->mCh[0].mEnabled || gScope->mCh[1].mEnabled || gScope->mCh[2].mEnabled || gScope->mCh[3].mEnabled)
    {
        ImVec2 mp = ImGui::GetMousePos();
        mp.x -= p.x;
        mp.y -= p.y;
        if (mp.x > 0 && mp.x < grid_size * uiScale &&
            mp.y > 0 && mp.y < grid_size * uiScale)
        {
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(p.x + mp.x, p.y),
                ImVec2(p.x + mp.x, p.y + grid_size * uiScale),
                0xff00ff00, 1 * uiScale);
            ImGui::BeginTooltip();
            int note = (int)(12 * log(32 * POW_2_3_4TH * (freqbins[(int)mp.x] / 440)) / log(2));            
            if (note < 0 || note > 127) note = -1;
            ImGui::Text("%3.3fHz%s%s", freqbins[(int)mp.x], note==-1?"":"\n", note==-1?"":gNotestr[note]);
            ImGui::EndTooltip();
        }
    }
}

#if 0
static int groups(ScopeData* gScope, double h)
{
    int count = 0;
    for (int i = 1; i < gScope->mPot / 4; i++)
    {
        if (gScope->fft1[i - 1] < h && gScope->fft1[i] > h)
            count++;
    }
    return count;
}

static void detect_fundamentals(ScopeData* gScope)
{
    // gScope->fft1[1..pot/4] has our mags
    double maxmag = 0;
    for (int i = 0; i < gScope->mPot / 4; i++)
        if (maxmag < gScope->fft1[i]) 
            maxmag = gScope->fft1[i];

    double minmag = 0;
    int count = 0;
    int iters = 0;
    double h = (minmag + maxmag) / 2;
    double step = h / 2;
    while (iters < 100 && count != 16)
    {
        count = groups(gScope, h);
        if (count < 16)
        { 
            h -= step;
        }
        else
        {
            h += step;
        }
        step /= 2;
        iters++;
    }
    char temp[1024];
    int ofs = 0;
    temp[0] = 0;
    const float gSamplerate = gScope->mSampleRate;
    float freqbin = gSamplerate / (float)(gScope->mPot / 2);

    int startbin = 0;
    for (int i = 2; i < gScope->mPot / 4; i++)
    {
        if (gScope->fft1[i - 1] < h && gScope->fft1[i] > h)
        {
            startbin = i;
        }
        if (gScope->fft1[i - 1] > h && gScope->fft1[i] < h)
        {
            double sum = 0;
            double magsum = 0;
            for (int j = startbin; j < i; j++)
            {
                sum += gScope->fft1[j];
                magsum += gScope->fft1[j] * j * freqbin;
            }
            if (sum != 0)
            {
                magsum /= sum;
                sum /= i - startbin;
                sum /= maxmag / 2; // normalize
                ofs += sprintf(temp + ofs, "%3.3f\t%3.3f\n", magsum, sum);
            }
        }
    }


    for (int i = 0; i < count; i++)
    {
    }
    ImGui::SetClipboardText(temp);
}
#endif

void do_show_scope_window(ScopeData* gScope, const float uiScale)
{
    // Data is updated live, so let's take local copies of critical stuff.
    int index = gScope->mIndex;

    ImGui::Begin("Scope", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::BeginChild("Channel options", ImVec2((4 * 25)*uiScale, (2 * 152 + 32) * uiScale));
    ImGui::Checkbox("###ea", &gScope->mCh[0].mEnabled); ImGui::SameLine();
    ImGui::Checkbox("###eb", &gScope->mCh[1].mEnabled); ImGui::SameLine();
    ImGui::Checkbox("###ec", &gScope->mCh[2].mEnabled); ImGui::SameLine();
    ImGui::Checkbox("###ed", &gScope->mCh[3].mEnabled);

    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[0]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[0]);
    if (ImGui::VSliderInt("###0a", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[0].mScaleSlider, -4, 4, ""))
    {
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            gScope->mCh[0].mScaleSlider = 0;
            gScope->mCh[0].mScale = scalesteps[4];
        }
        else
        {
            gScope->mCh[0].mScale = scalesteps[gScope->mCh[0].mScaleSlider + 4];
        }
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", scaletexts[gScope->mCh[0].mScaleSlider + 4]);
        ImGui::EndTooltip();
    }
    ImGui::PopStyleColor(2);  ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[1]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[1]);
    if (ImGui::VSliderInt("###1a", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[1].mScaleSlider, -4, 4, ""))
    {
        gScope->mCh[1].mScale = scalesteps[gScope->mCh[1].mScaleSlider + 4];
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", scaletexts[gScope->mCh[1].mScaleSlider + 4]);
        ImGui::EndTooltip();
    }
    ImGui::PopStyleColor(2);  ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[2]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[2]);
    if (ImGui::VSliderInt("###2a", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[2].mScaleSlider, -4, 4, ""))
    {
        gScope->mCh[2].mScale = scalesteps[gScope->mCh[2].mScaleSlider + 4];
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", scaletexts[gScope->mCh[2].mScaleSlider + 4]);
        ImGui::EndTooltip();
    }
    ImGui::PopStyleColor(2);  ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[3]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[3]);
    if (ImGui::VSliderInt("###3a", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[3].mScaleSlider, -4, 4, ""))
    {
        gScope->mCh[3].mScale = scalesteps[gScope->mCh[3].mScaleSlider + 4];
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", scaletexts[gScope->mCh[3].mScaleSlider + 4]);
        ImGui::EndTooltip();
    }
    ImGui::PopStyleColor(2);

    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[0]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[0]); ImGui::VSliderFloat("###0b", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[0].mOffset, -2, 2, ""); ImGui::PopStyleColor(2);   ImGui::SameLine();
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%3.3f", gScope->mCh[0].mOffset);
        ImGui::EndTooltip();
    }
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[1]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[1]); ImGui::VSliderFloat("###1b", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[1].mOffset, -2, 2, ""); ImGui::PopStyleColor(2);   ImGui::SameLine();
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%3.3f", gScope->mCh[1].mOffset);
        ImGui::EndTooltip();
    }
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[2]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[2]); ImGui::VSliderFloat("###2b", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[2].mOffset, -2, 2, ""); ImGui::PopStyleColor(2);   ImGui::SameLine();
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%3.3f", gScope->mCh[2].mOffset);
        ImGui::EndTooltip();
    }
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, gScope->colors[3]); ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, gScope->colors[3]); ImGui::VSliderFloat("###3b", ImVec2(19 * uiScale, 150 * uiScale), &gScope->mCh[3].mOffset, -2, 2, ""); ImGui::PopStyleColor(2);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%3.3f", gScope->mCh[3].mOffset);
        ImGui::EndTooltip();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
    ImGui::BeginChild("Scope and scroll", ImVec2(grid_size * uiScale, (grid_size + 24)* uiScale));
    ImGui::BeginChild("Scope proper", ImVec2(grid_size * uiScale, grid_size * uiScale));

    if (gScope->mDisplay == 0)
        scope_time(gScope, uiScale, index);
    if (gScope->mDisplay == 1)
        scope_freq(gScope, uiScale, index);
    /*
    if (gScope->mDisplay == 2 || gScope->mDisplay == 3)
        scope_plot(gScope, uiScale, index);
    */

    ImGui::EndChild();
    ImGui::PopStyleColor(1);
    /*
    if (gScope->mDisplay == 1)
    {
        if (ImGui::BeginPopupContextItem("Freq Context"))
        {
            if (ImGui::BeginMenu("Experimental.."))
            {
                if (ImGui::MenuItem("Detect and copy fundamental frequencies"))
                {
                    detect_fundamentals(gScope);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Averaging.."))
            {
                if (ImGui::MenuItem("1x"))
                    gScope->fft.average = 1;
                if (ImGui::MenuItem("4x"))
                    gScope->fft.average = 4;
                if (ImGui::MenuItem("16x"))
                    gScope->fft.average = 16;
                if (ImGui::MenuItem("64x"))
                    gScope->fft.average = 64;
                if (ImGui::MenuItem("256x"))
                    gScope->fft.average = 256;
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
    }
    //context_menu(1, 1, 1);
    */

    if (gScope->mMode)
    {
        ImGui::SetNextItemWidth(grid_size * uiScale);
        ImGui::SliderFloat("###scroll", &gScope->mScroll, -10.0f, 0.0f, "%.3f s");
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0xff3f3f3f);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, 0xff3f3f3f);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, 0xff3f3f3f);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, 0xff7f7f7f);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, 0xff7f7f7f);
        ImGui::SetNextItemWidth(grid_size * uiScale);
        float x = gScope->mScroll;
        ImGui::SliderFloat("###scroll", &x, -10.0f, 0.0f, "%.3f s");
        ImGui::PopStyleColor(5);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Scope options", ImVec2((4 * 21) * uiScale, 364 * uiScale));
    if (ImGui::VSliderInt("###0a", ImVec2(19 * uiScale, 155 * uiScale), &gScope->mTimeScaleSlider, -2, 2, ""))
    {
        gScope->mTimeScale = timescalesteps[gScope->mTimeScaleSlider + 2];
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s", timescaletext[gScope->mTimeScaleSlider + 2]);
        ImGui::EndTooltip();
    }
    ImGui::SameLine();
    ImGui::BeginChild("moderadio", ImVec2(100 * uiScale, 155 * uiScale));
    if (ImGui::RadioButton("Time", gScope->mDisplay == 0)) gScope->mDisplay = 0;
    if (ImGui::RadioButton("Freq", gScope->mDisplay == 1)) gScope->mDisplay = 1;
    /*
    if (ImGui::RadioButton("X,X'", gScope->mDisplay == 2)) gScope->mDisplay = 2;
    if (ImGui::RadioButton("X,Y", gScope->mDisplay == 3)) gScope->mDisplay = 3;
    */
    ImGui::Separator();
    ImGui::Text("FFT");
    if (ImGui::RadioButton("1x", gScope->mFFTZoom == 0)) gScope->mFFTZoom = 0;
    if (ImGui::RadioButton("2x", gScope->mFFTZoom == 1)) gScope->mFFTZoom = 1;
    if (ImGui::RadioButton("4x", gScope->mFFTZoom == 2)) gScope->mFFTZoom = 2;
    if (ImGui::RadioButton("8x", gScope->mFFTZoom == 3)) gScope->mFFTZoom = 3;
    ImGui::EndChild();
    char temp[64];
    sprintf(temp, "Sync ch %d###sc", gScope->mSyncChannel + 1);
    if (ImGui::Button(temp, ImVec2(80 * uiScale, 20 * uiScale)))
        gScope->mSyncChannel = (gScope->mSyncChannel + 1) % 4;
    const char* syncmodes[3] = { "^", "v", "off" };
    sprintf(temp, "Sync %s###sm", syncmodes[gScope->mSyncMode]);
    if (ImGui::Button(temp, ImVec2(80 * uiScale, 20 * uiScale)))
        gScope->mSyncMode = (gScope->mSyncMode + 1) % 3;

    if (gScope->mMode == 0)
    {
        if (ImGui::Button("Pause", ImVec2(80 * uiScale, 20 * uiScale)))
            gScope->mMode = 1;
        ImGui::Text("Nudge (ms)");
        ImGui::PushStyleColor(ImGuiCol_Button, 0xff3f3f3f);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xff3f3f3f);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff3f3f3f);
        ImGui::Button("-0.1", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::SameLine();
        ImGui::Button("+0.1", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::Button("-1", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::SameLine();
        ImGui::Button("+1", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::Button("-10", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::SameLine();
        ImGui::Button("+10", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::Button("-100", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::SameLine();
        ImGui::Button("+100", ImVec2(38 * uiScale, 20 * uiScale));
        ImGui::PopStyleColor(3);
    }
    else
    {
        if (ImGui::Button("Capture", ImVec2(80 * uiScale, 20 * uiScale)))
            gScope->mMode = 0;
        ImGui::Text("Nudge (ms)");
        if (ImGui::Button("-0.1", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll -= 0.0001f;
        }
        ImGui::SameLine();
        if (ImGui::Button("+0.1", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll += 0.0001f;
        }
        if (ImGui::Button("-1", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll -= 0.001f;
        }
        ImGui::SameLine();
        if (ImGui::Button("+1", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll += 0.001f;
        }
        if (ImGui::Button("-10", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll -= 0.01f;
        }
        ImGui::SameLine();
        if (ImGui::Button("+10", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll += 0.01f;
        }
        if (ImGui::Button("-100", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll -= 0.1f;
        }
        ImGui::SameLine();
        if (ImGui::Button("+100", ImVec2(38 * uiScale, 20 * uiScale)))
        {
            gScope->mScroll += 0.1f;
        }
    }
    ImGui::EndChild();

    ImGui::End();

}
