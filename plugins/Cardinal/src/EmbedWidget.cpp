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

#if defined(ARCH_LIN) && !defined(HEADLESS)
# define HAVE_X11
#endif

#ifdef HAVE_X11
# include <sys/types.h>
# include <X11/Xatom.h>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/extensions/shape.h>
#endif

#include "EmbedWidget.hpp"

static void offsetToXY(const Vec offset, int& x, int& y)
{
    if (offset.x > 0.0f)
        x = static_cast<int>(offset.x + 0.5f);
    else if (offset.x < 0.0f)
        x = static_cast<int>(offset.x - 0.5f);
    else
        x = 0;

    if (offset.y > 0.0f)
        y = static_cast<int>(offset.y + 0.5f);
    else if (offset.y < 0.0f)
        y = static_cast<int>(offset.y - 0.5f);
    else
        y = 0;
}

struct EmbedWidget::PrivateData {
   #ifdef HAVE_X11
    ::Display* display = nullptr;
    ::Window window = 0;
   #endif

    int lastX = 0;
    int lastY = 0;
    uint lastWidth = 0;
    uint lastHeight = 0;
    bool browserWasVisible = false;

    PrivateData(const Vec size)
    {
        const uint width = size.x;
        const uint height = size.y;

       #ifdef HAVE_X11
        display = XOpenDisplay(nullptr);
        DISTRHO_SAFE_ASSERT_RETURN(display != nullptr,);

        int ignore = 0;
        if (XShapeQueryExtension(display, &ignore, &ignore) == False)
        {
            XCloseDisplay(display);
            display = nullptr;
            async_dialog_message("XShape extension unsupported, cannot use embed widgets");
            return;
        }

        const ::Window rootWindow = RootWindow(display, DefaultScreen(display));

        window = XCreateSimpleWindow(display, rootWindow, 0, 0, width, height, 0, 0, 0);
        DISTRHO_SAFE_ASSERT_RETURN(window != 0,);

        XSizeHints sizeHints = {};
        sizeHints.flags      = PMinSize | PMaxSize;
        sizeHints.min_width  = width;
        sizeHints.max_width  = width;
        sizeHints.min_height = height;
        sizeHints.max_height = height;
        XSetNormalHints(display, window, &sizeHints);
        XStoreName(display, window, "EmbedWidget");
       #endif

        lastWidth = width;
        lastHeight = height;
    }

    ~PrivateData()
    {
       #ifdef HAVE_X11
        if (display == nullptr)
            return;

        if (window != 0)
            XDestroyWindow(display, window);

        XCloseDisplay(display);
       #endif
    }

    void embedIntoRack(const uintptr_t nativeWindowId, const Rect rect)
    {
        int x, y;
        offsetToXY(rect.pos, x, y);
        lastX = x;
        lastY = y;

        const uint width = rect.size.x;
        const uint height = rect.size.y;

        if (lastWidth != width || lastHeight != height)
        {
            lastWidth = width;
            lastHeight = height;
           #ifdef HAVE_X11
            XResizeWindow(display, window, width, height);
           #endif
        }

       #ifdef HAVE_X11
        DISTRHO_SAFE_ASSERT_RETURN(window != 0,);

        XReparentWindow(display, window, nativeWindowId, x, y);
        setClipMask(x, y, width, height);
       #endif
    }

    void removeFromRack()
    {
       #ifdef HAVE_X11
        DISTRHO_SAFE_ASSERT_RETURN(window != 0,);

        const ::Window rootWindow = RootWindow(display, DefaultScreen(display));

        XReparentWindow(display, window, rootWindow, 0, 0);
        XSync(display, False);
       #endif
    }

    void show()
    {
       #ifdef HAVE_X11
        DISTRHO_SAFE_ASSERT_RETURN(window != 0,);

        XMapRaised(display, window);
        XSync(display, False);
       #endif
    }

    void hide()
    {
       #ifdef HAVE_X11
        DISTRHO_SAFE_ASSERT_RETURN(window != 0,);

        XUnmapWindow(display, window);
       #endif
    }

    void step(const Rect rect)
    {
        int x, y;
        offsetToXY(rect.pos, x, y);

        const bool browserVisible = APP->scene->browser->visible;
        const uint width = rect.size.x;
        const uint height = rect.size.y;

        const bool diffBrowser = browserWasVisible != browserVisible;
        const bool diffPos = lastX != x || lastY != y;
        const bool diffSize = lastWidth != width || lastHeight != height;

        if (diffBrowser)
            browserWasVisible = browserVisible;

        /**/ if (diffPos && diffSize)
        {
            lastX = x;
            lastY = y;
            lastWidth = width;
            lastHeight = height;
        }
        else if (diffPos)
        {
            lastX = x;
            lastY = y;
           #ifdef HAVE_X11
           #endif
        }
        else if (diffSize)
        {
            lastWidth = width;
            lastHeight = height;
           #ifdef HAVE_X11
           #endif
        }

       #ifdef HAVE_X11
        if (window == 0)
            return;

        if (diffBrowser || diffPos || diffSize)
        {
            /**/ if (diffPos && diffSize)
                XMoveResizeWindow(display, window, x, y, width, height);
            else if (diffPos)
                XMoveWindow(display, window, x, y);
            else if (diffSize)
                XResizeWindow(display, window, width, height);

            setClipMask(x, y, width, height);
        }

        for (XEvent event; XPending(display) > 0;)
            XNextEvent(display, &event);
       #endif
    }

   #ifdef HAVE_X11
    void setClipMask(const int x, const int y, const uint width, const uint height)
    {
        const size_t len = width*height/4;
        uchar* const data = new uchar[len];

        if (browserWasVisible)
        {
            // allow nothing
            std::memset(data, 0xff, sizeof(uchar)*len);
        }
        else
        {
            // allow everything
            std::memset(data, 0, sizeof(uchar)*len);

            // crop out menuBar
            const int menuBarSize = APP->scene->menuBar->box.size.y * APP->window->pixelRatio;
            const uint normy = (y < menuBarSize ? std::max(menuBarSize - y, 0) : 0);
            for (uint i=0, j=0, d=0; i < width * height; ++i, ++j)
            {
                if (i >= normy * width)
                    break;

                if (i == 0)
                {
                }
                else if ((j % width) == 0)
                {
                    j = 0;
                    ++d;
                }
                else if ((j % 8) == 0)
                {
                    ++d;
                }

                DISTRHO_SAFE_ASSERT_BREAK(d < len);

                const uint v = (j % 8);
                data[d] |= 1 << v;
            }
        }

        const ::Pixmap pixmap = XCreatePixmapFromBitmapData(display, window, (char*)data, width, height, 0, 1, 1);
        delete[] data;

        XShapeCombineMask(display, window, ShapeBounding, 0, 0, pixmap, ShapeSet);
        XFreePixmap(display, pixmap);
    }
   #endif
};

EmbedWidget::EmbedWidget(const Vec size)
    : pData(new PrivateData(size))
{
    box.size = size;
}

EmbedWidget::~EmbedWidget()
{
    delete pData;
}

void EmbedWidget::embedIntoRack(const uintptr_t nativeWindowId)
{
    pData->embedIntoRack(nativeWindowId, getAbsoluteRect());
}

void EmbedWidget::removeFromRack()
{
    pData->removeFromRack();
}

void EmbedWidget::show()
{
    pData->show();
}

void EmbedWidget::hide()
{
    pData->hide();
}

uintptr_t EmbedWidget::getNativeWindowId() const
{
   #ifdef HAVE_X11
    return pData->window;
   #else
    return 0;
   #endif
}

void EmbedWidget::draw(const DrawArgs& args)
{
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGB(0, 0, 0));
    nvgFill(args.vg);
}

void EmbedWidget::step()
{
    pData->step(getAbsoluteRect());
}

Rect EmbedWidget::getAbsoluteRect()
{
    return Rect(getAbsoluteOffset({}), box.size.mult(getAbsoluteZoom()));
}
