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
          resizing(false)
    {
        resetArea();
    }

    /** Overloaded constructor, will fetch the window from an existing top-level widget. */
    explicit ResizeHandle(TopLevelWidget* const tlw)
        : TopLevelWidget(tlw->getWindow()),
          handleSize(16),
          resizing(false)
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
        const GraphicsContext& context(getGraphicsContext());
        const double lineWidth = 1.0 * getScaleFactor();

#ifdef DGL_OPENGL
        glUseProgram(0);
        glMatrixMode(GL_MODELVIEW);
#endif

        // draw white lines, 1px wide
        Color(1.0f, 1.0f, 1.0f).setFor(context);
        l1.draw(context, lineWidth);
        l2.draw(context, lineWidth);
        l3.draw(context, lineWidth);

        // draw black lines, offset by 1px and 1px wide
        Color(0.0f, 0.0f, 0.0f).setFor(context);
        Line<double> l1b(l1), l2b(l2), l3b(l3);
        l1b.moveBy(lineWidth, lineWidth);
        l2b.moveBy(lineWidth, lineWidth);
        l3b.moveBy(lineWidth, lineWidth);
        l1b.draw(context, lineWidth);
        l2b.draw(context, lineWidth);
        l3b.draw(context, lineWidth);
    }

    bool onMouse(const MouseEvent& ev) override
    {
        if (ev.button != 1)
            return false;

        if (ev.press && area.contains(ev.pos))
        {
            resizing = true;
            resizingSize = Size<double>(getWidth(), getHeight());
            lastResizePoint = ev.pos;
            return true;
        }

        if (resizing && ! ev.press)
        {
            resizing = false;
            return true;
        }

        return false;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        if (! resizing)
            return false;

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
    Line<double> l1, l2, l3;
    uint handleSize;

    // event handling state
    bool resizing;
    Point<double> lastResizePoint;
    Size<double> resizingSize;

    void resetArea()
    {
        const double scaleFactor = getScaleFactor();
        const uint margin = 0.0 * scaleFactor;
        const uint size = handleSize * scaleFactor;

        area = Rectangle<uint>(getWidth() - size - margin,
                               getHeight() - size - margin,
                               size, size);

        recreateLines(area.getX(), area.getY(), size);
    }

    void recreateLines(const uint x, const uint y, const uint size)
    {
        uint linesize = size;
        uint offset = 0;

        // 1st line, full diagonal size
        l1.setStartPos(x + size, y);
        l1.setEndPos(x, y + size);

        // 2nd line, bit more to the right and down, cropped
        offset += size / 3;
        linesize -= size / 3;
        l2.setStartPos(x + linesize + offset, y + offset);
        l2.setEndPos(x + offset, y + linesize + offset);

        // 3rd line, even more right and down
        offset += size / 3;
        linesize -= size / 3;
        l3.setStartPos(x + linesize + offset, y + offset);
        l3.setEndPos(x + offset, y + linesize + offset);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResizeHandle)
};

END_NAMESPACE_DGL
