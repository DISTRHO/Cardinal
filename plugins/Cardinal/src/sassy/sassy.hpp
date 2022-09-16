/*
 * Sassy scope exported API
 * Copyright (C) 2022 Filipe Coelho <falktx@falktx.com>
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
 * This file contains a substantial amount of code from Sassy Audio Spreadsheet
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

#pragma once

#include "fftreal/FFTReal.h"

// int gFFTAverage = 1;
// int gSamplerate;
// float mUIScale;
// // gScope

struct ScopeData {
    int mIndex = 0;
    int mSampleRate = 0;
    float mScroll = 0;
    float mTimeScale = 0.01f;
    int mTimeScaleSlider = 0;
    int mSyncMode = 0;
    int mSyncChannel = 0;
    int mMode = 0;
    int mDisplay = 0;
    int mFFTZoom = 0;
    int mPot = 0;
    bool darkMode = true;
    float fft1[65536 * 2];
    float fft2[65536 * 2];
    float ffta[65536 * 2];
    unsigned int colors[4] = {
        0xffc0c0c0,
        0xffa0a0ff,
        0xffffa0a0,
        0xff30d0d0
    };

    struct Channel {
        bool mEnabled = true;
        float mScale = 1.0f / 5.0f;
        int mScaleSlider = 0;
        float mOffset = 0;
        float* mData = nullptr;

        ~Channel()
        {
            delete[] mData;
        }

        void realloc(const int sampleRate)
        {
            mData = new float[sampleRate * 10];
            memset(mData, 0, sizeof(float) * sampleRate * 10);
        }
    } mCh[4];

    struct {
        int average;
        ffft::FFTReal<float>* obj16;
        ffft::FFTReal<float>* obj32;
        ffft::FFTReal<float>* obj64;
        ffft::FFTReal<float>* obj128;
        ffft::FFTReal<float>* obj256;
        ffft::FFTReal<float>* obj512;
        ffft::FFTReal<float>* obj1024;
        ffft::FFTReal<float>* obj2048;
        ffft::FFTReal<float>* obj4096;
        ffft::FFTReal<float>* obj8192;
        ffft::FFTReal<float>* obj16384;
        ffft::FFTReal<float>* obj32768;
        ffft::FFTReal<float>* obj65536;
    } fft;

    void realloc(const int sampleRate)
    {
        mIndex = 0;
        mSampleRate = sampleRate;

        for (int i = 0; i < 4; i++)
            mCh[i].realloc(sampleRate);
    }

    inline void probe(float data1, float data2, float data3, float data4)
    {
        // since probe has several channels, need to deal with index here
        if (mMode == 0)
        {
            mCh[0].mData[mIndex] = data1;
            mCh[1].mData[mIndex] = data2;
            mCh[2].mData[mIndex] = data3;
            mCh[3].mData[mIndex] = data4;
            mIndex = (mIndex + 1) % (mSampleRate * 10);
        }
    }
};

void do_show_scope_window(ScopeData* scope, float uiScale);
