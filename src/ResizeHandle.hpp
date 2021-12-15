/*
 * Resize handle for DPF
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include "TopLevelWidget.hpp"
#include "../dgl/Color.hpp"

START_NAMESPACE_DGL

/** Resize handle for DPF windows, will sit on bottom-right. */
class ResizeHandle : public TopLevelWidget
{
public:
    /** Constructor for placing this handle on top of a window. */
    explicit ResizeHandle(Window& window)
        : TopLevelWidget(window),
          handleSize(16),
          hasCursor(false),
          isResizing(false)
    {
        resetArea();
    }

    /** Overloaded constructor, will fetch the window from an existing top-level widget. */
    explicit ResizeHandle(TopLevelWidget* const tlw)
        : TopLevelWidget(tlw->getWindow()),
          handleSize(16),
          hasCursor(false),
          isResizing(false)
    {
        resetArea();
    }

    /** Set the handle size, minimum 16. */
    void setHandleSize(const uint size)
    {
        handleSize = std::max(16u, size);
        resetArea();
    }

protected:
    void onDisplay() override
    {
        /* Nothing here, we purposefully avoid drawing anything.
         * The resize handle is on the plugin UI directly. */
    }

    bool onMouse(const MouseEvent& ev) override
    {
        if (ev.button != 1)
            return false;

        if (ev.press && area.contains(ev.pos))
        {
            isResizing = true;
            resizingSize = Size<double>(getWidth(), getHeight());
            lastResizePoint = ev.pos;
            return true;
        }

        if (isResizing && ! ev.press)
        {
            isResizing = false;
            recheckCursor(ev.pos);
            return true;
        }

        return false;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        if (! isResizing)
        {
            recheckCursor(ev.pos);
            return false;
        }

        const Size<double> offset(ev.pos.getX() - lastResizePoint.getX(),
                                  ev.pos.getY() - lastResizePoint.getY());

        resizingSize += offset;
        lastResizePoint = ev.pos;

        // TODO min width, min height
        const uint minWidth = 16;
        const uint minHeight = 16;

        if (resizingSize.getWidth() < minWidth)
            resizingSize.setWidth(minWidth);
        if (resizingSize.getWidth() > 16384)
            resizingSize.setWidth(16384);
        if (resizingSize.getHeight() < minHeight)
            resizingSize.setHeight(minHeight);
        if (resizingSize.getHeight() > 16384)
            resizingSize.setHeight(16384);

        setSize(resizingSize.getWidth(), resizingSize.getHeight());
        return true;
    }

    void onResize(const ResizeEvent& ev) override
    {
        TopLevelWidget::onResize(ev);
        resetArea();
    }

private:
    Rectangle<uint> area;
    uint handleSize;

    // event handling state
    bool hasCursor, isResizing;
    Point<double> lastResizePoint;
    Size<double> resizingSize;

    void recheckCursor(const Point<double>& pos)
    {
        const bool shouldHaveCursor = area.contains(pos);

        if (shouldHaveCursor == hasCursor)
            return;

        hasCursor = shouldHaveCursor;
        setCursor(shouldHaveCursor ? kMouseCursorDiagonal : kMouseCursorArrow);
    }

    void resetArea()
    {
        const double scaleFactor = getScaleFactor();
        const uint size = handleSize * scaleFactor;

        area = Rectangle<uint>(getWidth() - size,
                               getHeight() - size,
                               size, size);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResizeHandle)
};

END_NAMESPACE_DGL
