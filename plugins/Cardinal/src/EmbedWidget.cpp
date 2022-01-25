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

    PrivateData(const Vec size)
    {
        const uint width = size.x;
        const uint height = size.y;

       #ifdef HAVE_X11
        display = XOpenDisplay(nullptr);
        DISTRHO_SAFE_ASSERT_RETURN(display != nullptr,);

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
        XMapRaised(display, window);
        XSync(display, False);

        d_stdout("this window is %lu", window);
       #endif
    }

    void hide()
    {
       #ifdef HAVE_X11
        DISTRHO_SAFE_ASSERT_RETURN(window != 0,);

        const ::Window rootWindow = RootWindow(display, DefaultScreen(display));

        XUnmapWindow(display, window);
        XReparentWindow(display, window, rootWindow, 0, 0);
       #endif
    }

    void step(const Rect rect)
    {
        int x, y;
        offsetToXY(rect.pos, x, y);

        const uint width = rect.size.x;
        const uint height = rect.size.y;

        const bool diffPos = (lastX != x || lastY != y);
        const bool diffSize = (lastWidth != width || lastHeight != height);

        if (diffPos && diffSize)
        {
            lastX = x;
            lastY = y;
            lastWidth = width;
            lastHeight = height;
           #ifdef HAVE_X11
            XMoveResizeWindow(display, window, x, y, width, height);
           #endif
        }
        else if (diffPos)
        {
            lastX = x;
            lastY = y;
           #ifdef HAVE_X11
            XMoveWindow(display, window, x, y);
           #endif
        }
        else if (diffSize)
        {
            lastWidth = width;
            lastHeight = height;
           #ifdef HAVE_X11
            XResizeWindow(display, window, width, height);
           #endif
        }

       #ifdef HAVE_X11
        if (window == 0)
            return;

        for (XEvent event; XPending(display) > 0;)
            XNextEvent(display, &event);
       #endif
    }
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

void EmbedWidget::step()
{
    pData->step(getAbsoluteRect());
}

Rect EmbedWidget::getAbsoluteRect()
{
    return Rect(getAbsoluteOffset({}), box.size.mult(getAbsoluteZoom()));
}
