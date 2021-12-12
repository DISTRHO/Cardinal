/*
Blendish - Blender 2.5 UI based theming functions for NanoVG

Copyright (c) 2014 Leonard Ritter <leonard.ritter@duangle.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <memory.h>
#include <math.h>
#include <blendish.h>

#ifdef _MSC_VER
    #pragma warning (disable: 4996) // Switch off security warnings
    #pragma warning (disable: 4100) // Switch off unreferenced formal parameter warnings
    #pragma warning (disable: 4244)
    #pragma warning (disable: 4305)
    #ifdef __cplusplus
    #define BND_INLINE inline
    #else
    #define BND_INLINE
    #endif

#include <float.h>

static float bnd_fminf ( float a, float b )
{
    return _isnan(a) ? b : ( _isnan(b) ? a : ((a < b) ? a : b));
}

static float bnd_fmaxf ( float a, float b )
{
    return _isnan(a) ? b : ( _isnan(b) ? a : ((a > b) ? a : b));
}

static double bnd_fmin ( double a, double b )
{
    return _isnan(a) ? b : ( _isnan(b) ? a : ((a < b) ? a : b));
}

static double bnd_fmax ( double a, double b )
{
    return _isnan(a) ? b : ( _isnan(b) ? a : ((a > b) ? a : b));
}

#else
    #define BND_INLINE static inline
    #define bnd_fminf(a, b) fminf(a, b)
    #define bnd_fmaxf(a, b) fmaxf(a, b)
    #define bnd_fmin(a, b) fmin(a, b)
    #define bnd_fmax(a, b) fmax(a, b)
#endif

////////////////////////////////////////////////////////////////////////////////

BND_INLINE float bnd_clamp(float v, float mn, float mx) {
    return (v > mx)?mx:(v < mn)?mn:v;
}

////////////////////////////////////////////////////////////////////////////////

// the initial theme
static BNDtheme bnd_theme = {
    // backgroundColor
    {{{ 0.447, 0.447, 0.447, 1.0 }}},
    // regularTheme
    {
        {{{ 0.098,0.098,0.098,1 }}}, // color_outline
        {{{ 0.098,0.098,0.098,1 }}}, // color_item
        {{{ 0.6,0.6,0.6,1 }}}, // color_inner
        {{{ 0.392,0.392,0.392,1 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        0, // shade_top
        0, // shade_down
    },
    // toolTheme
    {
        {{{ 0.098,0.098,0.098,1 }}}, // color_outline
        {{{ 0.098,0.098,0.098,1 }}}, // color_item
        {{{ 0.6,0.6,0.6,1 }}}, // color_inner
        {{{ 0.392,0.392,0.392,1 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        15, // shade_top
        -15, // shade_down
    },
    // radioTheme
    {
        {{{ 0,0,0,1 }}}, // color_outline
        {{{ 1,1,1,1 }}}, // color_item
        {{{ 0.275,0.275,0.275,1 }}}, // color_inner
        {{{ 0.337,0.502,0.761,1 }}}, // color_inner_selected
        BND_COLOR_TEXT_SELECTED, // color_text
        BND_COLOR_TEXT, // color_text_selected
        15, // shade_top
        -15, // shade_down
    },
    // textFieldTheme
    {
        {{{ 0.098,0.098,0.098,1 }}}, // color_outline
        {{{ 0.353, 0.353, 0.353,1 }}}, // color_item
        {{{ 0.6, 0.6, 0.6,1 }}}, // color_inner
        {{{ 0.6, 0.6, 0.6,1 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        0, // shade_top
        25, // shade_down
    },
    // optionTheme
    {
        {{{ 0,0,0,1 }}}, // color_outline
        {{{ 1,1,1,1 }}}, // color_item
        {{{ 0.275,0.275,0.275,1 }}}, // color_inner
        {{{ 0.275,0.275,0.275,1 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        15, // shade_top
        -15, // shade_down
    },
    // choiceTheme
    {
        {{{ 0,0,0,1 }}}, // color_outline
        {{{ 1,1,1,1 }}}, // color_item
        {{{ 0.275,0.275,0.275,1 }}}, // color_inner
        {{{ 0.275,0.275,0.275,1 }}}, // color_inner_selected
        BND_COLOR_TEXT_SELECTED, // color_text
        {{{ 0.8,0.8,0.8,1 }}}, // color_text_selected
        15, // shade_top
        -15, // shade_down
    },
    // numberFieldTheme
    {
        {{{ 0.098,0.098,0.098,1 }}}, // color_outline
        {{{ 0.353, 0.353, 0.353,1 }}}, // color_item
        {{{ 0.706, 0.706, 0.706,1 }}}, // color_inner
        {{{ 0.6, 0.6, 0.6,1 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        -20, // shade_top
        0, // shade_down
    },
    // sliderTheme
    {
        {{{ 0.098,0.098,0.098,1 }}}, // color_outline
        {{{ 0.502,0.502,0.502,1 }}}, // color_item
        {{{ 0.706, 0.706, 0.706,1 }}}, // color_inner
        {{{ 0.6, 0.6, 0.6,1 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        -20, // shade_top
        0, // shade_down
    },
    // scrollBarTheme
    {
        {{{ 0.196,0.196,0.196,1 }}}, // color_outline
        {{{ 0.502,0.502,0.502,1 }}}, // color_item
        {{{ 0.314, 0.314, 0.314,0.706 }}}, // color_inner
        {{{ 0.392, 0.392, 0.392,0.706 }}}, // color_inner_selected
        BND_COLOR_TEXT, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        5, // shade_top
        -5, // shade_down
    },
    // tooltipTheme
    {
        {{{ 0,0,0,1 }}}, // color_outline
        {{{ 0.392,0.392,0.392,1 }}}, // color_item
        {{{ 0.098, 0.098, 0.098, 0.902 }}}, // color_inner
        {{{ 0.176, 0.176, 0.176, 0.902 }}}, // color_inner_selected
        {{{ 0.627, 0.627, 0.627, 1 }}}, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        0, // shade_top
        0, // shade_down
    },
    // menuTheme
    {
        {{{ 0,0,0,1 }}}, // color_outline
        {{{ 0.392,0.392,0.392,1 }}}, // color_item
        {{{ 0.098, 0.098, 0.098, 0.902 }}}, // color_inner
        {{{ 0.176, 0.176, 0.176, 0.902 }}}, // color_inner_selected
        {{{ 0.627, 0.627, 0.627, 1 }}}, // color_text
        BND_COLOR_TEXT_SELECTED, // color_text_selected
        0, // shade_top
        0, // shade_down
    },
    // menuItemTheme
    {
        {{{ 0,0,0,1 }}}, // color_outline
        {{{ 0.675,0.675,0.675,0.502 }}}, // color_item
        {{{ 0,0,0,0 }}}, // color_inner
        {{{ 0.337,0.502,0.761,1 }}}, // color_inner_selected
        BND_COLOR_TEXT_SELECTED, // color_text
        BND_COLOR_TEXT, // color_text_selected
        38, // shade_top
        0, // shade_down
    },
    // nodeTheme
    {
        {{{ 0.945,0.345,0,1 }}}, // nodeSelectedColor
        {{{ 0,0,0,1 }}}, // wiresColor
        {{{ 0.498,0.439,0.439,1 }}}, // textSelectedColor
        {{{ 1,0.667,0.251,1 }}}, // activeNodeColor
        {{{ 1,1,1,1 }}}, // wireSelectColor
        {{{ 0.608,0.608,0.608,0.627 }}}, // nodeBackdropColor
        5, // noodleCurving
    },
};

////////////////////////////////////////////////////////////////////////////////

void bndSetTheme(BNDtheme theme) {
    bnd_theme = theme;
}

const BNDtheme *bndGetTheme() {
    return &bnd_theme;
}

// the handle to the image containing the icon sheet
static int bnd_icon_image = -1;

void bndSetIconImage(int image) {
    bnd_icon_image = image;
}

// the handle to the UI font
static int bnd_font = -1;

void bndSetFont(int font) {
    bnd_font = font;
}

////////////////////////////////////////////////////////////////////////////////

void bndLabel(NVGcontext *ctx,
    float x, float y, float w, float h, int iconid, const char *label) {
    bndIconLabelValue(ctx,x,y,w,h,iconid,
        bnd_theme.regularTheme.textColor, BND_LEFT,
        BND_LABEL_FONT_SIZE, label, NULL);
}

void bndToolButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state,
    int iconid, const char *label) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_TOOL_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.toolTheme, state, 1);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.toolTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,iconid,
        bndTextColor(&bnd_theme.toolTheme, state), BND_CENTER,
        BND_LABEL_FONT_SIZE, label, NULL);
}

void bndRadioButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state,
    int iconid, const char *label) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_OPTION_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.radioTheme, state, 1);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.radioTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,iconid,
        bndTextColor(&bnd_theme.radioTheme, state), BND_CENTER,
        BND_LABEL_FONT_SIZE, label, NULL);
}

int bndTextFieldTextPosition(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, const char *text, int px, int py) {
    return bndIconLabelTextPosition(ctx, x, y, w, h,
        iconid, BND_LABEL_FONT_SIZE, text, px, py);
}

void bndTextField(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state,
    int iconid, const char *text, int cbegin, int cend) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_TEXT_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.textFieldTheme, state, 0);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.textFieldTheme.outlineColor));
    if (state != BND_ACTIVE) {
        cend = -1;
    }
    bndIconLabelCaret(ctx,x,y,w,h,iconid,
        bndTextColor(&bnd_theme.textFieldTheme, state), BND_LABEL_FONT_SIZE,
        text, bnd_theme.textFieldTheme.itemColor, cbegin, cend);
}

void bndOptionButton(NVGcontext *ctx,
    float x, float y, float w, float h, BNDwidgetState state,
    const char *label) {
    float ox, oy;
    NVGcolor shade_top, shade_down;

    ox = x;
    oy = y+h-BND_OPTION_HEIGHT-3;

    bndBevelInset(ctx,ox,oy,
        BND_OPTION_WIDTH,BND_OPTION_HEIGHT,
        BND_OPTION_RADIUS,BND_OPTION_RADIUS);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.optionTheme, state, 1);
    bndInnerBox(ctx,ox,oy,
        BND_OPTION_WIDTH,BND_OPTION_HEIGHT,
        BND_OPTION_RADIUS,BND_OPTION_RADIUS,BND_OPTION_RADIUS,BND_OPTION_RADIUS,
        shade_top, shade_down);
    bndOutlineBox(ctx,ox,oy,
        BND_OPTION_WIDTH,BND_OPTION_HEIGHT,
        BND_OPTION_RADIUS,BND_OPTION_RADIUS,BND_OPTION_RADIUS,BND_OPTION_RADIUS,
        bndTransparent(bnd_theme.optionTheme.outlineColor));
    if (state == BND_ACTIVE) {
        bndCheck(ctx,ox,oy, bndTransparent(bnd_theme.optionTheme.itemColor));
    }
    bndIconLabelValue(ctx,x+12,y,w-12,h,-1,
        bndTextColor(&bnd_theme.optionTheme, state), BND_LEFT,
        BND_LABEL_FONT_SIZE, label, NULL);
}

void bndChoiceButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state,
    int iconid, const char *label) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_OPTION_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.choiceTheme, state, 1);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.choiceTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,iconid,
        bndTextColor(&bnd_theme.choiceTheme, state), BND_LEFT,
        BND_LABEL_FONT_SIZE, label, NULL);
    bndUpDownArrow(ctx,x+w-10,y+10,5,
        bndTransparent(bnd_theme.choiceTheme.itemColor));
}

void bndColorButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, NVGcolor color) {
    float cr[4];
    bndSelectCorners(cr, BND_TOOL_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], color, color);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.toolTheme.outlineColor));
}

void bndNumberField(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state,
    const char *label, const char *value) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_NUMBER_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.numberFieldTheme, state, 0);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.numberFieldTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,-1,
        bndTextColor(&bnd_theme.numberFieldTheme, state), BND_CENTER,
        BND_LABEL_FONT_SIZE, label, value);
    bndArrow(ctx,x+8,y+10,-BND_NUMBER_ARROW_SIZE,
        bndTransparent(bnd_theme.numberFieldTheme.itemColor));
    bndArrow(ctx,x+w-8,y+10,BND_NUMBER_ARROW_SIZE,
        bndTransparent(bnd_theme.numberFieldTheme.itemColor));
}

void bndSlider(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state,
    float progress, const char *label, const char *value) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_NUMBER_RADIUS, flags);
    bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.sliderTheme, state, 0);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);

    if (state == BND_ACTIVE) {
        shade_top = bndOffsetColor(
            bnd_theme.sliderTheme.itemColor, bnd_theme.sliderTheme.shadeTop);
        shade_down = bndOffsetColor(
            bnd_theme.sliderTheme.itemColor, bnd_theme.sliderTheme.shadeDown);
    } else {
        shade_top = bndOffsetColor(
            bnd_theme.sliderTheme.itemColor, bnd_theme.sliderTheme.shadeDown);
        shade_down = bndOffsetColor(
            bnd_theme.sliderTheme.itemColor, bnd_theme.sliderTheme.shadeTop);
    }
    nvgScissor(ctx,x,y,8+(w-8)*bnd_clamp(progress,0,1),h);
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    nvgResetScissor(ctx);

    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.sliderTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,-1,
        bndTextColor(&bnd_theme.sliderTheme, state), BND_CENTER,
        BND_LABEL_FONT_SIZE, label, value);
}

void bndScrollBar(NVGcontext *ctx,
    float x, float y, float w, float h, BNDwidgetState state,
    float offset, float size) {

    bndBevelInset(ctx,x,y,w,h,
        BND_SCROLLBAR_RADIUS, BND_SCROLLBAR_RADIUS);
    bndInnerBox(ctx,x,y,w,h,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        bndOffsetColor(
            bnd_theme.scrollBarTheme.innerColor, 3*bnd_theme.scrollBarTheme.shadeDown),
        bndOffsetColor(
            bnd_theme.scrollBarTheme.innerColor, 3*bnd_theme.scrollBarTheme.shadeTop));
    bndOutlineBox(ctx,x,y,w,h,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        bndTransparent(bnd_theme.scrollBarTheme.outlineColor));

    NVGcolor itemColor = bndOffsetColor(
        bnd_theme.scrollBarTheme.itemColor,
        (state == BND_ACTIVE)?BND_SCROLLBAR_ACTIVE_SHADE:0);

    bndScrollHandleRect(&x,&y,&w,&h,offset,size);

    bndInnerBox(ctx,x,y,w,h,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        bndOffsetColor(
            itemColor, 3*bnd_theme.scrollBarTheme.shadeTop),
        bndOffsetColor(
            itemColor, 3*bnd_theme.scrollBarTheme.shadeDown));
    bndOutlineBox(ctx,x,y,w,h,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        BND_SCROLLBAR_RADIUS,BND_SCROLLBAR_RADIUS,
        bndTransparent(bnd_theme.scrollBarTheme.outlineColor));
}

void bndMenuBackground(NVGcontext *ctx,
    float x, float y, float w, float h, int flags) {
    float cr[4];
    NVGcolor shade_top, shade_down;

    bndSelectCorners(cr, BND_MENU_RADIUS, flags);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.menuTheme,
        BND_DEFAULT, 0);
    bndInnerBox(ctx,x,y,w,h+1,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h+1,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.menuTheme.outlineColor));
    bndDropShadow(ctx,x,y,w,h,BND_MENU_RADIUS,
        BND_SHADOW_FEATHER,BND_SHADOW_ALPHA);
}

void bndTooltipBackground(NVGcontext *ctx, float x, float y, float w, float h) {
    NVGcolor shade_top, shade_down;

    bndInnerColors(&shade_top, &shade_down, &bnd_theme.tooltipTheme,
        BND_DEFAULT, 0);
    bndInnerBox(ctx,x,y,w,h+1,
        BND_MENU_RADIUS,BND_MENU_RADIUS,BND_MENU_RADIUS,BND_MENU_RADIUS,
        shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h+1,
        BND_MENU_RADIUS,BND_MENU_RADIUS,BND_MENU_RADIUS,BND_MENU_RADIUS,
        bndTransparent(bnd_theme.tooltipTheme.outlineColor));
    bndDropShadow(ctx,x,y,w,h,BND_MENU_RADIUS,
        BND_SHADOW_FEATHER,BND_SHADOW_ALPHA);
}

void bndMenuLabel(NVGcontext *ctx,
    float x, float y, float w, float h, int iconid, const char *label) {
    bndIconLabelValue(ctx,x,y,w,h,iconid,
        bnd_theme.menuTheme.textColor, BND_LEFT,
        BND_LABEL_FONT_SIZE, label, NULL);
}

void bndMenuItem(NVGcontext *ctx,
    float x, float y, float w, float h, BNDwidgetState state,
    int iconid, const char *label) {
    if (state != BND_DEFAULT) {
        bndInnerBox(ctx,x,y,w,h,0,0,0,0,
            bndOffsetColor(bnd_theme.menuItemTheme.innerSelectedColor,
                bnd_theme.menuItemTheme.shadeTop),
            bndOffsetColor(bnd_theme.menuItemTheme.innerSelectedColor,
                bnd_theme.menuItemTheme.shadeDown));
        state = BND_ACTIVE;
    }
    bndIconLabelValue(ctx,x,y,w,h,iconid,
        bndTextColor(&bnd_theme.menuItemTheme, state), BND_LEFT,
        BND_LABEL_FONT_SIZE, label, NULL);
}

void bndNodePort(NVGcontext *ctx, float x, float y, BNDwidgetState state,
    NVGcolor color) {
    nvgBeginPath(ctx);
    nvgCircle(ctx, x, y, BND_NODE_PORT_RADIUS);
    nvgStrokeColor(ctx,bnd_theme.nodeTheme.wiresColor);
    nvgStrokeWidth(ctx,1.0f);
    nvgStroke(ctx);
    nvgFillColor(ctx,(state != BND_DEFAULT)?
        bndOffsetColor(color, BND_HOVER_SHADE):color);
    nvgFill(ctx);
}

void bndColoredNodeWire(NVGcontext *ctx, float x0, float y0, float x1, float y1,
    NVGcolor color0, NVGcolor color1) {
    float length = bnd_fmaxf(fabsf(x1 - x0),fabsf(y1 - y0));
    float delta = length*(float)bnd_theme.nodeTheme.noodleCurving/10.0f;

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x0, y0);
    nvgBezierTo(ctx,
        x0 + delta, y0,
        x1 - delta, y1,
        x1, y1);
    NVGcolor colorw = bnd_theme.nodeTheme.wiresColor;
    colorw.a = (color0.a<color1.a)?color0.a:color1.a;
    nvgStrokeColor(ctx, colorw);
    nvgStrokeWidth(ctx, BND_NODE_WIRE_OUTLINE_WIDTH);
    nvgStroke(ctx);
    nvgStrokePaint(ctx, nvgLinearGradient(ctx,
        x0, y0, x1, y1,
        color0,
        color1));
    nvgStrokeWidth(ctx,BND_NODE_WIRE_WIDTH);
    nvgStroke(ctx);
}

void bndNodeWire(NVGcontext *ctx, float x0, float y0, float x1, float y1,
    BNDwidgetState state0, BNDwidgetState state1) {
    bndColoredNodeWire(ctx, x0, y0, x1, y1,
        bndNodeWireColor(&bnd_theme.nodeTheme, state0),
        bndNodeWireColor(&bnd_theme.nodeTheme, state1));
}

void bndNodeBackground(NVGcontext *ctx, float x, float y, float w, float h,
    BNDwidgetState state, int iconid, const char *label, NVGcolor titleColor) {
    bndInnerBox(ctx,x,y,w,BND_NODE_TITLE_HEIGHT+2,
        BND_NODE_RADIUS,BND_NODE_RADIUS,0,0,
        bndTransparent(bndOffsetColor(titleColor, BND_BEVEL_SHADE)),
        bndTransparent(titleColor));
    bndInnerBox(ctx,x,y+BND_NODE_TITLE_HEIGHT-1,w,h+2-BND_NODE_TITLE_HEIGHT,
        0,0,BND_NODE_RADIUS,BND_NODE_RADIUS,
        bndTransparent(bnd_theme.nodeTheme.nodeBackdropColor),
        bndTransparent(bnd_theme.nodeTheme.nodeBackdropColor));
    bndNodeIconLabel(ctx,
        x+BND_NODE_ARROW_AREA_WIDTH,y,
        w-BND_NODE_ARROW_AREA_WIDTH-BND_NODE_MARGIN_SIDE,BND_NODE_TITLE_HEIGHT,
        iconid, bnd_theme.regularTheme.textColor,
        bndOffsetColor(titleColor, BND_BEVEL_SHADE),
        BND_LEFT, BND_LABEL_FONT_SIZE, label);
    NVGcolor arrowColor;
    NVGcolor borderColor;
    switch(state) {
    default:
    case BND_DEFAULT: {
        borderColor = nvgRGBf(0,0,0);
        arrowColor = bndOffsetColor(titleColor, -BND_BEVEL_SHADE);
    } break;
    case BND_HOVER: {
        borderColor = bnd_theme.nodeTheme.nodeSelectedColor;
        arrowColor = bnd_theme.nodeTheme.nodeSelectedColor;
    } break;
    case BND_ACTIVE: {
        borderColor = bnd_theme.nodeTheme.activeNodeColor;
        arrowColor = bnd_theme.nodeTheme.nodeSelectedColor;
    } break;
    }
    bndOutlineBox(ctx,x,y,w,h+1,
        BND_NODE_RADIUS,BND_NODE_RADIUS,BND_NODE_RADIUS,BND_NODE_RADIUS,
        bndTransparent(borderColor));
    /*
    bndNodeArrowDown(ctx,
        x + BND_NODE_MARGIN_SIDE, y + BND_NODE_TITLE_HEIGHT-4,
        BND_NODE_ARROW_SIZE, arrowColor);
    */
    bndDropShadow(ctx,x,y,w,h,BND_NODE_RADIUS,
        BND_SHADOW_FEATHER,BND_SHADOW_ALPHA);
}

void bndSplitterWidgets(NVGcontext *ctx, float x, float y, float w, float h) {
    NVGcolor insetLight = bndTransparent(
        bndOffsetColor(bnd_theme.backgroundColor, BND_SPLITTER_SHADE));
    NVGcolor insetDark = bndTransparent(
        bndOffsetColor(bnd_theme.backgroundColor, -BND_SPLITTER_SHADE));
    NVGcolor inset = bndTransparent(bnd_theme.backgroundColor);

    float x2 = x+w;
    float y2 = y+h;

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y2-13);
    nvgLineTo(ctx, x+13, y2);
    nvgMoveTo(ctx, x, y2-9);
    nvgLineTo(ctx, x+9, y2);
    nvgMoveTo(ctx, x, y2-5);
    nvgLineTo(ctx, x+5, y2);

    nvgMoveTo(ctx, x2-11, y);
    nvgLineTo(ctx, x2, y+11);
    nvgMoveTo(ctx, x2-7, y);
    nvgLineTo(ctx, x2, y+7);
    nvgMoveTo(ctx, x2-3, y);
    nvgLineTo(ctx, x2, y+3);

    nvgStrokeColor(ctx, insetDark);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y2-11);
    nvgLineTo(ctx, x+11, y2);
    nvgMoveTo(ctx, x, y2-7);
    nvgLineTo(ctx, x+7, y2);
    nvgMoveTo(ctx, x, y2-3);
    nvgLineTo(ctx, x+3, y2);

    nvgMoveTo(ctx, x2-13, y);
    nvgLineTo(ctx, x2, y+13);
    nvgMoveTo(ctx, x2-9, y);
    nvgLineTo(ctx, x2, y+9);
    nvgMoveTo(ctx, x2-5, y);
    nvgLineTo(ctx, x2, y+5);

    nvgStrokeColor(ctx, insetLight);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y2-12);
    nvgLineTo(ctx, x+12, y2);
    nvgMoveTo(ctx, x, y2-8);
    nvgLineTo(ctx, x+8, y2);
    nvgMoveTo(ctx, x, y2-4);
    nvgLineTo(ctx, x+4, y2);

    nvgMoveTo(ctx, x2-12, y);
    nvgLineTo(ctx, x2, y+12);
    nvgMoveTo(ctx, x2-8, y);
    nvgLineTo(ctx, x2, y+8);
    nvgMoveTo(ctx, x2-4, y);
    nvgLineTo(ctx, x2, y+4);

    nvgStrokeColor(ctx, inset);
    nvgStroke(ctx);
}

void bndJoinAreaOverlay(NVGcontext *ctx, float x, float y, float w, float h,
    int vertical, int mirror) {

    if (vertical) {
        float u = w;
        w = h; h = u;
    }

    float s = (w<h)?w:h;

    float x0,y0,x1,y1;
    if (mirror) {
        x0 = w;
        y0 = h;
        x1 = 0;
        y1 = 0;
        s = -s;
    } else {
        x0 = 0;
        y0 = 0;
        x1 = w;
        y1 = h;
    }

    float yc = (y0+y1)*0.5f;
    float s2 = s/2.0f;
    float s4 = s/4.0f;
    float s8 = s/8.0f;
    float x4 = x0+s4;

    float points[][2] = {
        { x0,y0 },
        { x1,y0 },
        { x1,y1 },
        { x0,y1 },
        { x0,yc+s8 },
        { x4,yc+s8 },
        { x4,yc+s4 },
        { x0+s2,yc },
        { x4,yc-s4 },
        { x4,yc-s8 },
        { x0,yc-s8 }
    };

    nvgBeginPath(ctx);
    int count = sizeof(points) / (sizeof(float)*2);
    nvgMoveTo(ctx,x+points[0][vertical&1],y+points[0][(vertical&1)^1]);
    for (int i = 1; i < count; ++i) {
        nvgLineTo(ctx,x+points[i][vertical&1],y+points[i][(vertical&1)^1]);
    }

    nvgFillColor(ctx, nvgRGBAf(0,0,0,0.3));
    nvgFill(ctx);
}

////////////////////////////////////////////////////////////////////////////////

float bndLabelWidth(NVGcontext *ctx, int iconid, const char *label) {
    int w = BND_PAD_LEFT + BND_PAD_RIGHT;
    if (iconid >= 0) {
        w += BND_ICON_SHEET_RES;
    }
    if (label && (bnd_font >= 0)) {
        nvgFontFaceId(ctx, bnd_font);
        nvgFontSize(ctx, BND_LABEL_FONT_SIZE);
        float bounds[4];
        nvgTextBoxBounds(ctx, 1, 1, INFINITY, label, NULL, bounds);
        w += bounds[2];
    }
    return w;
}

float bndLabelHeight(NVGcontext *ctx, int iconid, const char *label, float width) {
	int h = BND_WIDGET_HEIGHT;
    width -= BND_TEXT_RADIUS*2;
    if (iconid >= 0) {
        width -= BND_ICON_SHEET_RES;
    }
    if (label && (bnd_font >= 0)) {
        nvgFontFaceId(ctx, bnd_font);
        nvgFontSize(ctx, BND_LABEL_FONT_SIZE);
        float bounds[4];
        nvgTextBoxBounds(ctx, 1, 1, width, label, NULL, bounds);
        int bh = (int)(bounds[3] - bounds[1]) + BND_TEXT_PAD_DOWN;
        if (bh > h)
        	h = bh;
    }
    return h;
}

////////////////////////////////////////////////////////////////////////////////

void bndRoundedBox(NVGcontext *ctx, float x, float y, float w, float h,
    float cr0, float cr1, float cr2, float cr3) {
    float d;

    w = bnd_fmaxf(0, w);
    h = bnd_fmaxf(0, h);
    d = bnd_fminf(w, h);

    nvgMoveTo(ctx, x,y+h*0.5f);
    nvgArcTo(ctx, x,y, x+w,y, bnd_fminf(cr0, d/2));
    nvgArcTo(ctx, x+w,y, x+w,y+h, bnd_fminf(cr1, d/2));
    nvgArcTo(ctx, x+w,y+h, x,y+h, bnd_fminf(cr2, d/2));
    nvgArcTo(ctx, x,y+h, x,y, bnd_fminf(cr3, d/2));
    nvgClosePath(ctx);
}

NVGcolor bndTransparent(NVGcolor color) {
    color.a *= BND_TRANSPARENT_ALPHA;
    return color;
}

NVGcolor bndOffsetColor(NVGcolor color, int delta) {
    float offset = (float)delta / 255.0f;
    return delta?(
        nvgRGBAf(
            bnd_clamp(color.r+offset,0,1),
            bnd_clamp(color.g+offset,0,1),
            bnd_clamp(color.b+offset,0,1),
            color.a)
    ):color;
}

void bndBevel(NVGcontext *ctx, float x, float y, float w, float h) {
    // Disable bevel
    return;
    nvgStrokeWidth(ctx, 1);

    x += 0.5f;
    y += 0.5f;
    w -= 1;
    h -= 1;

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y+h);
    nvgLineTo(ctx, x+w, y+h);
    nvgLineTo(ctx, x+w, y);
    nvgStrokeColor(ctx, bndTransparent(
        bndOffsetColor(bnd_theme.backgroundColor, -BND_BEVEL_SHADE)));
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y+h);
    nvgLineTo(ctx, x, y);
    nvgLineTo(ctx, x+w, y);
    nvgStrokeColor(ctx, bndTransparent(
        bndOffsetColor(bnd_theme.backgroundColor, BND_BEVEL_SHADE)));
    nvgStroke(ctx);
}

void bndBevelInset(NVGcontext *ctx, float x, float y, float w, float h,
    float cr2, float cr3) {
    // Disable bevel
    return;
    float d;

    y -= 0.5f;
    d = bnd_fminf(w, h);
    cr2 = bnd_fminf(cr2, d/2);
    cr3 = bnd_fminf(cr3, d/2);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x+w,y+h-cr2);
    nvgArcTo(ctx, x+w,y+h, x,y+h, cr2);
    nvgArcTo(ctx, x,y+h, x,y, cr3);

    NVGcolor bevelColor = bndOffsetColor(bnd_theme.backgroundColor,
        BND_INSET_BEVEL_SHADE);

    nvgStrokeWidth(ctx, 1);
    nvgStrokePaint(ctx,
        nvgLinearGradient(ctx,
            x,y+h-bnd_fmaxf(cr2,cr3)-1,
            x,y+h-1,
        nvgRGBAf(bevelColor.r, bevelColor.g, bevelColor.b, 0),
        bevelColor));
    nvgStroke(ctx);
}

void bndBackground(NVGcontext *ctx, float x, float y, float w, float h) {
    nvgBeginPath(ctx);
    nvgRect(ctx, x, y, w, h);
    nvgFillColor(ctx, bnd_theme.backgroundColor);
    nvgFill(ctx);
}

void bndIcon(NVGcontext *ctx, float x, float y, int iconid) {
    int ix, iy, u, v;
    if (bnd_icon_image < 0) return; // no icons loaded

    ix = iconid & 0xff;
    iy = (iconid>>8) & 0xff;
    u = BND_ICON_SHEET_OFFSET_X + ix*BND_ICON_SHEET_GRID;
    v = BND_ICON_SHEET_OFFSET_Y + iy*BND_ICON_SHEET_GRID;

    nvgBeginPath(ctx);
    nvgRect(ctx,x,y,BND_ICON_SHEET_RES,BND_ICON_SHEET_RES);
    nvgFillPaint(ctx,
        nvgImagePattern(ctx,x-u,y-v,
        BND_ICON_SHEET_WIDTH,
        BND_ICON_SHEET_HEIGHT,
        0,bnd_icon_image,1));
    nvgFill(ctx);
}

void bndDropShadow(NVGcontext *ctx, float x, float y, float w, float h,
    float r, float feather, float alpha) {

    nvgBeginPath(ctx);
    y += feather;
    h -= feather;

    nvgMoveTo(ctx, x-feather, y-feather);
    nvgLineTo(ctx, x, y-feather);
    nvgLineTo(ctx, x, y+h-feather);
    nvgArcTo(ctx, x,y+h,x+r,y+h,r);
    nvgArcTo(ctx, x+w,y+h,x+w,y+h-r,r);
    nvgLineTo(ctx, x+w, y-feather);
    nvgLineTo(ctx, x+w+feather, y-feather);
    nvgLineTo(ctx, x+w+feather, y+h+feather);
    nvgLineTo(ctx, x-feather, y+h+feather);
    nvgClosePath(ctx);

    nvgFillPaint(ctx, nvgBoxGradient(ctx,
        x - feather*0.5f,y - feather*0.5f,
        w + feather,h+feather,
        r+feather*0.5f,
        feather,
        nvgRGBAf(0,0,0,alpha*alpha),
        nvgRGBAf(0,0,0,0)));
    nvgFill(ctx);
}

void bndInnerBox(NVGcontext *ctx, float x, float y, float w, float h,
    float cr0, float cr1, float cr2, float cr3,
    NVGcolor shade_top, NVGcolor shade_down) {
    nvgBeginPath(ctx);
    bndRoundedBox(ctx,x+1,y+1,w-2,h-3,bnd_fmaxf(0,cr0-1),
        bnd_fmaxf(0,cr1-1),bnd_fmaxf(0,cr2-1),bnd_fmaxf(0,cr3-1));
    nvgFillPaint(ctx,((h-2)>w)?
        nvgLinearGradient(ctx,x,y,x+w,y,shade_top,shade_down):
        nvgLinearGradient(ctx,x,y,x,y+h,shade_top,shade_down));
    nvgFill(ctx);
}

void bndOutlineBox(NVGcontext *ctx, float x, float y, float w, float h,
    float cr0, float cr1, float cr2, float cr3, NVGcolor color) {
    nvgBeginPath(ctx);
    bndRoundedBox(ctx,x+0.5f,y+0.5f,w-1,h-2,cr0,cr1,cr2,cr3);
    nvgStrokeColor(ctx,color);
    nvgStrokeWidth(ctx,1);
    nvgStroke(ctx);
}

void bndSelectCorners(float *radiuses, float r, int flags) {
    radiuses[0] = (flags & BND_CORNER_TOP_LEFT)?0:r;
    radiuses[1] = (flags & BND_CORNER_TOP_RIGHT)?0:r;
    radiuses[2] = (flags & BND_CORNER_DOWN_RIGHT)?0:r;
    radiuses[3] = (flags & BND_CORNER_DOWN_LEFT)?0:r;
}

void bndInnerColors(
    NVGcolor *shade_top, NVGcolor *shade_down,
    const BNDwidgetTheme *theme, BNDwidgetState state, int flipActive) {

    switch(state) {
    default:
    case BND_DEFAULT: {
        *shade_top = bndOffsetColor(theme->innerColor, theme->shadeTop);
        *shade_down = bndOffsetColor(theme->innerColor, theme->shadeDown);
    } break;
    case BND_HOVER: {
        NVGcolor color = bndOffsetColor(theme->innerColor, BND_HOVER_SHADE);
        *shade_top = bndOffsetColor(color, theme->shadeTop);
        *shade_down = bndOffsetColor(color, theme->shadeDown);
    } break;
    case BND_ACTIVE: {
        *shade_top = bndOffsetColor(theme->innerSelectedColor,
            flipActive?theme->shadeDown:theme->shadeTop);
        *shade_down = bndOffsetColor(theme->innerSelectedColor,
            flipActive?theme->shadeTop:theme->shadeDown);
    } break;
    }
}

NVGcolor bndTextColor(const BNDwidgetTheme *theme, BNDwidgetState state) {
    return (state == BND_ACTIVE)?theme->textSelectedColor:theme->textColor;
}

void bndIconLabelValue(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, NVGcolor color, int align, float fontsize, const char *label,
    const char *value) {
    float pleft = BND_PAD_LEFT;
    if (label) {
        if (iconid >= 0) {
            bndIcon(ctx,x+4,y+2,iconid);
            pleft += BND_ICON_SHEET_RES;
        }

        if (bnd_font < 0) return;
        nvgFontFaceId(ctx, bnd_font);
        nvgFontSize(ctx, fontsize);
        nvgBeginPath(ctx);
        nvgFillColor(ctx, color);
        if (value) {
            float label_width = nvgTextBounds(ctx, 1, 1, label, NULL, NULL);
            float sep_width = nvgTextBounds(ctx, 1, 1,
                BND_LABEL_SEPARATOR, NULL, NULL);

            nvgTextAlign(ctx, NVG_ALIGN_LEFT|NVG_ALIGN_BASELINE);
            x += pleft;
            if (align == BND_CENTER) {
                float width = label_width + sep_width
                    + nvgTextBounds(ctx, 1, 1, value, NULL, NULL);
                x += ((w-BND_PAD_RIGHT-pleft)-width)*0.5f;
            }
            y += BND_WIDGET_HEIGHT-BND_TEXT_PAD_DOWN;
            nvgText(ctx, x, y, label, NULL);
            x += label_width;
            nvgText(ctx, x, y, BND_LABEL_SEPARATOR, NULL);
            x += sep_width;
            nvgText(ctx, x, y, value, NULL);
        } else {
            nvgTextAlign(ctx,
                (align==BND_LEFT)?(NVG_ALIGN_LEFT|NVG_ALIGN_BASELINE):
                (NVG_ALIGN_CENTER|NVG_ALIGN_BASELINE));
            nvgTextBox(ctx,x+pleft,y+BND_WIDGET_HEIGHT-BND_TEXT_PAD_DOWN,
                w-BND_PAD_RIGHT-pleft,label, NULL);
        }
    } else if (iconid >= 0) {
        bndIcon(ctx,x+2,y+2,iconid);
    }
}

void bndNodeIconLabel(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, NVGcolor color, NVGcolor shadowColor,
    int align, float fontsize, const char *label) {
    if (label && (bnd_font >= 0)) {
        nvgFontFaceId(ctx, bnd_font);
        nvgFontSize(ctx, fontsize);
        nvgBeginPath(ctx);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT|NVG_ALIGN_BASELINE);
        nvgFillColor(ctx, shadowColor);
        nvgFontBlur(ctx, BND_NODE_TITLE_FEATHER);
        nvgTextBox(ctx,x+1,y+h+3-BND_TEXT_PAD_DOWN,
            w,label, NULL);
        nvgFillColor(ctx, color);
        nvgFontBlur(ctx, 0);
        nvgTextBox(ctx,x,y+h+2-BND_TEXT_PAD_DOWN,
            w,label, NULL);
    }
    if (iconid >= 0) {
        bndIcon(ctx,x+w-BND_ICON_SHEET_RES,y+3,iconid);
    }
}

int bndIconLabelTextPosition(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, float fontsize, const char *label, int px, int py) {
    float bounds[4];
    float pleft = BND_TEXT_RADIUS;
    if (!label) return -1;
    if (iconid >= 0)
        pleft += BND_ICON_SHEET_RES;

    if (bnd_font < 0) return -1;

    x += pleft;
    y += BND_WIDGET_HEIGHT - BND_TEXT_PAD_DOWN;

    nvgFontFaceId(ctx, bnd_font);
    nvgFontSize(ctx, fontsize);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);

    w -= BND_TEXT_RADIUS + pleft;

    float asc, desc, lh;
    static NVGtextRow rows[BND_MAX_ROWS];
    int nrows = nvgTextBreakLines(
        ctx, label, NULL, w, rows, BND_MAX_ROWS);
    if (nrows == 0) return 0;
    nvgTextBoxBounds(ctx, x, y, w, label, NULL, bounds);
    nvgTextMetrics(ctx, &asc, &desc, &lh);

    // calculate vertical position
    int row = bnd_clamp((int)((float)(py - bounds[1]) / lh), 0, nrows - 1);
    // search horizontal position
    static NVGglyphPosition glyphs[BND_MAX_GLYPHS];
    int nglyphs = nvgTextGlyphPositions(
        ctx, x, y, rows[row].start, rows[row].end, glyphs, BND_MAX_GLYPHS);
    int col, p = 0;
    for (col = 0; col < nglyphs && glyphs[col].x < px; ++col)
        p = glyphs[col].str - label;
    // see if we should move one character further
    if (col > 0 && col < nglyphs && glyphs[col].x - px < px - glyphs[col - 1].x)
        p = glyphs[col].str - label;
    return p;
}

static void bndCaretPosition(NVGcontext *ctx, float x, float y,
    float desc, float lineHeight, const char *caret, NVGtextRow *rows,int nrows,
    int *cr, float *cx, float *cy) {
    static NVGglyphPosition glyphs[BND_MAX_GLYPHS];
    int r,nglyphs;
    for (r=0; r < nrows-1 && rows[r].end < caret; ++r);
    *cr = r;
    *cx = x;
    *cy = y-lineHeight-desc + r*lineHeight;
    if (nrows == 0) return;
    *cx = rows[r].minx;
    nglyphs = nvgTextGlyphPositions(
        ctx, x, y, rows[r].start, rows[r].end, glyphs, BND_MAX_GLYPHS);
    for (int i=0; i < nglyphs; ++i) {
        *cx=glyphs[i].x;
        if (glyphs[i].str == caret) break;
    }
}

void bndIconLabelCaret(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, NVGcolor color, float fontsize, const char *label,
    NVGcolor caretcolor, int cbegin, int cend) {
    float pleft = BND_TEXT_RADIUS;
    if (!label) return;
    if (iconid >= 0) {
        bndIcon(ctx,x+4,y+2,iconid);
        pleft += BND_ICON_SHEET_RES;
    }

    if (bnd_font < 0) return;

    x+=pleft;
    y+=BND_WIDGET_HEIGHT-BND_TEXT_PAD_DOWN;

    nvgFontFaceId(ctx, bnd_font);
    nvgFontSize(ctx, fontsize);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT|NVG_ALIGN_BASELINE);

    w -= BND_TEXT_RADIUS+pleft;

    if (cend >= cbegin) {
        int c0r,c1r;
        float c0x,c0y,c1x,c1y;
        float desc,lh;
        static NVGtextRow rows[BND_MAX_ROWS];
        int nrows = nvgTextBreakLines(
            ctx, label, label+cend+1, w, rows, BND_MAX_ROWS);
        nvgTextMetrics(ctx, NULL, &desc, &lh);

        bndCaretPosition(ctx, x, y, desc, lh, label+cbegin,
            rows, nrows, &c0r, &c0x, &c0y);
        bndCaretPosition(ctx, x, y, desc, lh, label+cend,
            rows, nrows, &c1r, &c1x, &c1y);

        nvgBeginPath(ctx);
        if (cbegin == cend) {
            nvgFillColor(ctx, nvgRGBf(0.337,0.502,0.761));
            nvgRect(ctx, c0x-1, c0y, 2, lh+1);
        } else {
            nvgFillColor(ctx, caretcolor);
            if (c0r == c1r) {
                nvgRect(ctx, c0x-1, c0y, c1x-c0x+1, lh+1);
            } else {
                int blk=c1r-c0r-1;
                nvgRect(ctx, c0x-1, c0y, x+w-c0x+1, lh+1);
                nvgRect(ctx, x, c1y, c1x-x+1, lh+1);

                if (blk)
                    nvgRect(ctx, x, c0y+lh, w, blk*lh+1);
            }
        }
        nvgFill(ctx);
    }

    nvgBeginPath(ctx);
    nvgFillColor(ctx, color);
    nvgTextBox(ctx,x,y,w,label, NULL);
}

void bndCheck(NVGcontext *ctx, float ox, float oy, NVGcolor color) {
    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx,2);
    nvgStrokeColor(ctx,color);
    nvgLineCap(ctx,NVG_BUTT);
    nvgLineJoin(ctx,NVG_MITER);
    nvgMoveTo(ctx,ox+4,oy+5);
    nvgLineTo(ctx,ox+7,oy+8);
    nvgLineTo(ctx,ox+14,oy+1);
    nvgStroke(ctx);
}

void bndArrow(NVGcontext *ctx, float x, float y, float s, NVGcolor color) {
    nvgBeginPath(ctx);
    nvgMoveTo(ctx,x,y);
    nvgLineTo(ctx,x-s,y+s);
    nvgLineTo(ctx,x-s,y-s);
    nvgClosePath(ctx);
    nvgFillColor(ctx,color);
    nvgFill(ctx);
}

void bndUpDownArrow(NVGcontext *ctx, float x, float y, float s, NVGcolor color) {
    float w;

    nvgBeginPath(ctx);
    w = 1.1f*s;
    nvgMoveTo(ctx,x,y-1);
    nvgLineTo(ctx,x+0.5*w,y-s-1);
    nvgLineTo(ctx,x+w,y-1);
    nvgClosePath(ctx);
    nvgMoveTo(ctx,x,y+1);
    nvgLineTo(ctx,x+0.5*w,y+s+1);
    nvgLineTo(ctx,x+w,y+1);
    nvgClosePath(ctx);
    nvgFillColor(ctx,color);
    nvgFill(ctx);
}

void bndNodeArrowDown(NVGcontext *ctx, float x, float y, float s, NVGcolor color) {
    float w;
    nvgBeginPath(ctx);
    w = 1.0f*s;
    nvgMoveTo(ctx,x,y);
    nvgLineTo(ctx,x+0.5*w,y-s);
    nvgLineTo(ctx,x-0.5*w,y-s);
    nvgClosePath(ctx);
    nvgFillColor(ctx,color);
    nvgFill(ctx);
}

void bndScrollHandleRect(float *x, float *y, float *w, float *h,
    float offset, float size) {
    size = bnd_clamp(size,0,1);
    offset = bnd_clamp(offset,0,1);
    if ((*h) > (*w)) {
        float hs = bnd_fmaxf(size*(*h), (*w)+1);
        *y = (*y) + ((*h)-hs)*offset;
        *h = hs;
    } else {
        float ws = bnd_fmaxf(size*(*w), (*h)-1);
        *x = (*x) + ((*w)-ws)*offset;
        *w = ws;
    }
}

NVGcolor bndNodeWireColor(const BNDnodeTheme *theme, BNDwidgetState state) {
    switch(state) {
        default:
        case BND_DEFAULT: return nvgRGBf(0.5f,0.5f,0.5f);
        case BND_HOVER: return theme->wireSelectColor;
        case BND_ACTIVE: return theme->activeNodeColor;
    }
}
