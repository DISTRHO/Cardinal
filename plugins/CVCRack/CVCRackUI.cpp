/*
 * DISTRHO CVCRack Plugin
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

#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class CVCRackUI : public UI
{
public:
    CVCRackUI()
        : UI(128, 512)
    {
        setGeometryConstraints(32, 128, false);
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * DSP/Plugin Callbacks */

   /**
      A parameter has changed on the plugin side.
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
    }

   /* --------------------------------------------------------------------------------------------------------
    * Widget Callbacks */

   /**
      The drawing function.
    */
    void onDisplay() override
    {
    }

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Set our UI class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CVCRackUI)
};

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
    return new CVCRackUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
