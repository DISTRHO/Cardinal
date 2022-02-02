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

#pragma once

#include "rack.hpp"

#ifdef NDEBUG
# undef DEBUG
#endif

using namespace rack;

struct CardinalLedDisplayChoice : LedDisplayChoice {
    bool alignTextCenter = true;

    CardinalLedDisplayChoice(const char* const label = nullptr)
    {
        color = nvgRGBf(0.76f, 0.11f, 0.22f);
        textOffset.y -= 4;

        if (label != nullptr)
            text = label;
    }

    void drawLayer(const DrawArgs& args, const int layer) override
    {
        if (layer == 1)
        {
            nvgFillColor(args.vg, color);
            nvgTextLetterSpacing(args.vg, 0.0f);

            if (alignTextCenter)
            {
                nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
                nvgText(args.vg, box.size.x * 0.5f, textOffset.y, text.c_str(), nullptr);
            }
            else
            {
                nvgTextAlign(args.vg, NVG_ALIGN_LEFT);
                nvgText(args.vg, textOffset.x, textOffset.y, text.c_str(), nullptr);
            }
        }

        Widget::drawLayer(args, layer);
    }
};

struct NanoKnob : Knob {
    static const int ringSize = 4;

    std::string displayLabel = "Level";
    std::string displayString = "0 dB";
    float normalizedValue = 0.5f;

    NanoKnob()
    {
        box.size = Vec(100, 100);
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer != 1)
            return Knob::drawLayer(args, layer);

        const float w = box.size.x;
        const float h = box.size.y;

        const int knobSize = std::min(w, h - BND_WIDGET_HEIGHT * 2) - ringSize;

        const int knobStartX = w / 2 - knobSize / 2;
        const int knobStartY = ringSize;
        const int knobCenterX = knobStartX + knobSize / 2;
        const int knobCenterY = knobStartY + knobSize / 2;

        const NVGcolor testing = nvgRGBf(0.76f, 0.11f, 0.22f);

        nvgLineCap(args.vg, NVG_ROUND);

        // outer ring value
        nvgBeginPath(args.vg);
        nvgArc(args.vg,
              knobCenterX,
              knobCenterY,
              knobSize / 2 + ringSize / 2 + 1,
              nvgDegToRad(135.0f),
              nvgDegToRad(135.0f) + nvgDegToRad(270.0f * normalizedValue),
              NVG_CW);
        nvgStrokeWidth(args.vg, ringSize);
        nvgStrokeColor(args.vg, testing);
        nvgStroke(args.vg);

        // simulate color bleeding
        nvgBeginPath(args.vg);
        nvgArc(args.vg,
              knobCenterX,
              knobCenterY,
              knobSize / 2 - 3,
              nvgDegToRad(135.0f),
              nvgDegToRad(135.0f) + nvgDegToRad(270.0f * normalizedValue),
              NVG_CW);
        nvgStrokeWidth(args.vg, 5);
        nvgStrokeColor(args.vg, nvgRGBAf(testing.r, testing.g, testing.b, 0.1f));
        nvgStroke(args.vg);

        // line indicator
        nvgStrokeWidth(args.vg, 2);
        nvgSave(args.vg);
        nvgTranslate(args.vg, knobCenterX, knobCenterY);
        nvgRotate(args.vg, nvgDegToRad(45.0f) + normalizedValue * nvgDegToRad(270.0f));
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, -2, knobSize / 2 - 9, 2, 6, 1);
        nvgClosePath(args.vg);
        nvgFillColor(args.vg, nvgRGBf(1.0f, 1.0f, 1.0f));
        nvgFill(args.vg);
        nvgRestore(args.vg);

        // adjusted from VCVRack's LightWidget.cpp
        if (const float halo = settings::haloBrightness)
        {
            float radius = knobSize * 0.5f;
            float oradius = radius + std::min(radius * 4.f, 15.f);

            NVGcolor icol = color::mult(nvgRGBAf(testing.r, testing.g, testing.b, 0.2f), halo);
            NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
            NVGpaint paint = nvgRadialGradient(args.vg, knobCenterX, knobCenterY, radius, oradius, icol, ocol);

            nvgBeginPath(args.vg);
            nvgRect(args.vg, knobCenterX - oradius, knobCenterY - oradius, 2 * oradius, 2 * oradius);
            nvgFillPaint(args.vg, paint);
            nvgFill(args.vg);
        }

        // bottom label (value)
        bndIconLabelValue(args.vg, 0, knobSize + ringSize, w, BND_WIDGET_HEIGHT, -1,
                          testing, BND_CENTER,
                          BND_LABEL_FONT_SIZE, displayString.c_str(), nullptr);

        Knob::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override
    {
        if (engine::ParamQuantity* const pq = getParamQuantity())
            normalizedValue = pq->getScaledValue();

        const float w = box.size.x;
        const float h = box.size.y;

        const int knobSize = std::min(w, h - BND_WIDGET_HEIGHT * 2) - ringSize;

        const int knobStartX = w / 2 - knobSize / 2;
        const int knobStartY = ringSize;
        const int knobCenterX = knobStartX + knobSize / 2;
        const int knobCenterY = knobStartY + knobSize / 2;

        // knob
        NVGcolor shade_top;
        NVGcolor shade_down;
        BNDwidgetState state;
        if (APP->event->getDraggedWidget() == this)
            state = BND_ACTIVE;
        else if (APP->event->getHoveredWidget() == this)
            state = BND_HOVER;
        else
            state = BND_DEFAULT;
        bndInnerColors(&shade_top, &shade_down, &bndGetTheme()->optionTheme, state, 0);

        // inner fill
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, knobCenterX, knobCenterY, knobSize / 2);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg,
                                            knobStartX,
                                            knobStartY,
                                            knobStartX,
                                            knobStartY + knobSize,
                                            shade_top,
                                            shade_down));
        nvgFill(args.vg);

        // inner fill border (inner)
        nvgBeginPath(args.vg);
        nvgArc(args.vg, knobCenterX, knobCenterY, knobSize / 2 - 1, nvgDegToRad(0.0f), nvgDegToRad(360.0f), NVG_CCW);
        nvgClosePath(args.vg);
        nvgStrokeWidth(args.vg, 1);
        nvgStrokeColor(args.vg, nvgRGBAf(0.5f, 0.5f, 0.5f, 0.4f));
        nvgStroke(args.vg);

        // inner fill border (outer)
        nvgBeginPath(args.vg);
        nvgArc(args.vg, knobCenterX, knobCenterY, knobSize / 2, nvgDegToRad(0.0f), nvgDegToRad(360.0f), NVG_CCW);
        nvgClosePath(args.vg);
        nvgStrokeWidth(args.vg, 1);
        nvgStrokeColor(args.vg, nvgRGBAf(0.0f, 0.0f, 0.0f, 0.4f));
        nvgStroke(args.vg);

        nvgLineCap(args.vg, NVG_ROUND);

        // outer ring background
        nvgBeginPath(args.vg);
        nvgArc(args.vg,
              knobCenterX,
              knobCenterY,
              knobSize / 2 + ringSize / 2 + 1,
              nvgDegToRad(135.0f),
              nvgDegToRad(45.0f),
              NVG_CW);
        nvgStrokeWidth(args.vg, ringSize);
        nvgStrokeColor(args.vg, nvgRGBf(0.5f, 0.5f, 0.5f));
        nvgStroke(args.vg);

        // bottom label (name)
        bndIconLabelValue(args.vg, 0, knobStartY + knobSize + BND_WIDGET_HEIGHT * 0.75f, w, BND_WIDGET_HEIGHT, -1,
                          SCHEME_WHITE, BND_CENTER,
                          BND_LABEL_FONT_SIZE, displayLabel.c_str(), nullptr);

        Knob::draw(args);
    }

    void onChange(const ChangeEvent&) override
    {
        engine::ParamQuantity* const pq = getParamQuantity();
        DISTRHO_SAFE_ASSERT_RETURN(pq != nullptr,);

        displayLabel = pq->getLabel();
        displayString = pq->getDisplayValueString() + pq->getUnit();
    }
};

struct NanoMeter : Widget {
    float gainMeterL = 0.0f;
    float gainMeterR = 0.0f;

    virtual void updateMeters() = 0;

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer != 1)
            return;

        const float usableHeight = box.size.y - 10.0f;

        // draw background
        nvgBeginPath(args.vg);
        nvgRect(args.vg,
                0,
                0,
                box.size.x,
                usableHeight);
        nvgFillColor(args.vg, nvgRGB(26, 26, 26));
        nvgFill(args.vg);

        nvgFillColor(args.vg, nvgRGBAf(0.76f, 0.11f, 0.22f, 0.5f));
        nvgStrokeColor(args.vg, nvgRGBf(0.76f, 0.11f, 0.22f));

        updateMeters();

        const float heightL = 1.0f + std::sqrt(gainMeterL) * (usableHeight - 1.0f);
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0f, usableHeight - heightL, box.size.x * 0.5f - 1.0f, heightL);
        nvgFill(args.vg);
        nvgStroke(args.vg);

        const float heightR = 1.0f + std::sqrt(gainMeterR) * (usableHeight - 1.0f);
        nvgBeginPath(args.vg);
        nvgRect(args.vg, box.size.x * 0.5f + 1.0f, usableHeight - heightR, box.size.x * 0.5f - 2.0f, heightR);
        nvgFill(args.vg);
        nvgStroke(args.vg);

        nvgLineCap(args.vg, NVG_ROUND);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 2.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 11.0f, usableHeight + 2.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 4.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 16.0f, usableHeight + 4.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 6.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 19.0f, usableHeight + 6.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 8.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 22.0f, usableHeight + 8.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 10.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 24.0f, usableHeight + 10.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 12.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 26.0f, usableHeight + 12.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 10.0f, usableHeight + 2.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 2.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 15.0f, usableHeight + 4.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 4.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 18.0f, usableHeight + 6.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 6.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 20.0f, usableHeight + 8.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 8.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 22.0f, usableHeight + 10.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 10.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 24.0f, usableHeight + 12.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 12.0f);
        nvgStroke(args.vg);
    }
};

struct OpenGlWidgetWithBrowserPreview : OpenGlWidget {
    NVGLUframebuffer* fb = nullptr;

    void draw(const DrawArgs& args) override
    {
        if (args.fb == nullptr)
            return OpenGlWidget::draw(args);

        // set oversample to current scale
        float trans[6];
        nvgCurrentTransform(args.vg, trans);
        oversample = std::max(1.0f, trans[0]);

        // recreate framebuffer
        deleteFramebuffer();
        fb = nvgluCreateFramebuffer(args.vg, box.size.x * oversample, box.size.y * oversample, 0);
        DISTRHO_SAFE_ASSERT_RETURN(fb != nullptr,);

        // draw our special framebuffer
        nvgluBindFramebuffer(fb);
        drawFramebufferForBrowserPreview();

        // reset to regular framebuffer
        nvgluBindFramebuffer(args.fb);

        // render image generated by our framebuffer
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0f, 0.0f, box.size.x, box.size.y);
        NVGpaint paint = nvgImagePattern(args.vg,
                                          0.0f, 0.0f, box.size.x, box.size.y,
                                          0.0f, fb->image, 1.0f);
        nvgFillPaint(args.vg, paint);
        nvgFill(args.vg);
    }

    void onContextDestroy(const ContextDestroyEvent& e) override
    {
        deleteFramebuffer();
        OpenGlWidget::onContextDestroy(e);
    }

    void deleteFramebuffer()
    {
        if (fb == nullptr)
            return;
        nvgluDeleteFramebuffer(fb);
        fb = nullptr;
    }

    virtual void drawFramebufferForBrowserPreview() = 0;
};
