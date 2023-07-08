/*
 * Dear ImGui for DPF, converted to VCV
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
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

#include "ImGuiWidget.hpp"
#include "DearImGui/imgui.h"
#include "DistrhoUtils.hpp"

#ifndef DGL_NO_SHARED_RESOURCES
# include "../../../dpf/dgl/src/Resources.hpp"
#endif

#if defined(DGL_USE_OPENGL3)
# include "DearImGui/imgui_impl_opengl3.h"
#else
# include "DearImGui/imgui_impl_opengl2.h"
#endif

static const char* GetClipboardTextFn(void*)
{
    return glfwGetClipboardString(nullptr);
}

static void SetClipboardTextFn(void*, const char* const text)
{
    glfwSetClipboardString(nullptr, text);
}

static void setupIO()
{
    ImGuiIO& io(ImGui::GetIO());
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.GetClipboardTextFn = GetClipboardTextFn;
    io.SetClipboardTextFn = SetClipboardTextFn;
}

struct ImGuiWidget::PrivateData {
    ImGuiContext* context = nullptr;
    bool created = false;
    bool darkMode = true;
    bool fontGenerated = false;
    bool useMonospacedFont = false;
    float originalScaleFactor = 0.0f;
    float scaleFactor = 0.0f;
    double lastFrameTime = 0.0;

    PrivateData()
    {
        IMGUI_CHECKVERSION();
        context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);
        setupIO();
    }

    ~PrivateData()
    {
        // this should not happen
        if (created)
        {
            ImGui::SetCurrentContext(context);
#if defined(DGL_USE_OPENGL3)
            ImGui_ImplOpenGL3_Shutdown();
#else
            ImGui_ImplOpenGL2_Shutdown();
#endif
        }

        ImGui::DestroyContext(context);
    }

    void generateFontIfNeeded()
    {
        if (fontGenerated)
            return;

        DISTRHO_SAFE_ASSERT_RETURN(scaleFactor != 0.0f,);

        fontGenerated = true;

        ImGuiIO& io(ImGui::GetIO());

        if (useMonospacedFont)
        {
            const std::string fontPath = asset::system("res/fonts/ShareTechMono-Regular.ttf");
            ImFontConfig fc;
            fc.OversampleH = 1;
            fc.OversampleV = 1;
            fc.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 13.0f * scaleFactor, &fc);
            io.Fonts->Build();
        }
        else
        {
#ifndef DGL_NO_SHARED_RESOURCES
            using namespace dpf_resources;
            ImFontConfig fc;
            fc.FontDataOwnedByAtlas = false;
            fc.OversampleH = 1;
            fc.OversampleV = 1;
            fc.PixelSnapH = true;
            io.Fonts->AddFontFromMemoryTTF((void*)dejavusans_ttf, dejavusans_ttf_size, 13.0f * scaleFactor, &fc);

            // extra fonts we can try loading for unicode support
            static const char* extraFontPathsToTry[] = {
               #if defined(ARCH_WIN)
                // TODO
                // "Meiryo.ttc",
               #elif defined(ARCH_MAC)
                // TODO
               #elif defined(ARCH_LIN)
                "/usr/share/fonts/opentype/noto/NotoSerifCJK-Regular.ttc",
               #endif
            };

            fc.FontDataOwnedByAtlas = true;
            fc.MergeMode = true;

            for (size_t i=0; i<ARRAY_SIZE(extraFontPathsToTry); ++i)
            {
                if (rack::system::exists(extraFontPathsToTry[i]))
                    io.Fonts->AddFontFromFileTTF(extraFontPathsToTry[i], 13.0f * scaleFactor, &fc,
                                                 io.Fonts->GetGlyphRangesJapanese());
            }

            io.Fonts->Build();
#endif
        }
    }

    void resetEverything(const bool doInit)
    {
        if (created)
        {
            ImGui::SetCurrentContext(context);
#if defined(DGL_USE_OPENGL3)
            ImGui_ImplOpenGL3_Shutdown();
#else
            ImGui_ImplOpenGL2_Shutdown();
#endif
            created = false;
        }

        fontGenerated = false;
        originalScaleFactor = 0.0f;
        scaleFactor = 0.0f;
        lastFrameTime = 0.0;
        ImGui::DestroyContext(context);

        context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);
        setupIO();

        if (doInit)
        {
#if defined(DGL_USE_OPENGL3)
            ImGui_ImplOpenGL3_Init();
#else
            ImGui_ImplOpenGL2_Init();
#endif
            created = true;
        }
    }

    void resetStyle()
    {
        ImGuiStyle& style(ImGui::GetStyle());

        style.FrameRounding = 4;
        style.ScaleAllSizes(scaleFactor);

        const ImVec4 color_Cardinal(0.76f, 0.11f, 0.22f, 1.f);
        const ImVec4 color_DimCardinal(171.f / 255.f, 54.f / 255.f, 73.f / 255.f, 1.f);

        ImVec4* const colors = style.Colors;
        colors[ImGuiCol_Text]             = ImVec4(1.f, 1.f, 1.f, 1.f);
        colors[ImGuiCol_TextDisabled]     = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
        colors[ImGuiCol_WindowBg]         = ImVec4(0.099f, 0.099f, 0.099f, 1.f);
        colors[ImGuiCol_FrameBg]          = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
        colors[ImGuiCol_FrameBgActive]    = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
        colors[ImGuiCol_TitleBgActive]    = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        colors[ImGuiCol_CheckMark]        = color_Cardinal;
        colors[ImGuiCol_SliderGrab]       = color_DimCardinal;
        colors[ImGuiCol_SliderGrabActive] = color_Cardinal;
        colors[ImGuiCol_Button]           = color_DimCardinal;
        colors[ImGuiCol_ButtonHovered]    = color_Cardinal;
        colors[ImGuiCol_ButtonActive]     = color_Cardinal;
        colors[ImGuiCol_TextSelectedBg]   = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
        colors[ImGuiCol_Header]           = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
        colors[ImGuiCol_HeaderHovered]    = color_DimCardinal;
        colors[ImGuiCol_HeaderActive]     = color_Cardinal;

        if (!settings::darkMode)
        {
            for (int c = 0; c < ImGuiCol_COUNT; ++c)
            {
                if (std::memcmp(&colors[c], &color_Cardinal, sizeof(color_Cardinal)) == 0)
                    continue;
                if (std::memcmp(&colors[c], &color_DimCardinal, sizeof(color_DimCardinal)) == 0)
                    continue;
                colors[c] = ImVec4(1.f - colors[c].x, 1.f - colors[c].y, 1.f - colors[c].z, colors[c].w);
            }
        }
    }
};

ImGuiWidget::ImGuiWidget()
    : imData(new PrivateData()) {}

ImGuiWidget::~ImGuiWidget()
{
    delete imData;
}

float ImGuiWidget::getScaleFactor() const noexcept
{
    return imData->scaleFactor;
}

void ImGuiWidget::onContextCreate(const ContextCreateEvent& e)
{
    OpenGlWidgetWithBrowserPreview::onContextCreate(e);
    DISTRHO_SAFE_ASSERT_RETURN(!imData->created,);

    ImGui::SetCurrentContext(imData->context);
#if defined(DGL_USE_OPENGL3)
    ImGui_ImplOpenGL3_Init();
#else
    ImGui_ImplOpenGL2_Init();
#endif
    imData->created = true;
}

void ImGuiWidget::onContextDestroy(const ContextDestroyEvent& e)
{
    if (imData->created)
    {
        ImGui::SetCurrentContext(imData->context);
#if defined(DGL_USE_OPENGL3)
        ImGui_ImplOpenGL3_Shutdown();
#else
        ImGui_ImplOpenGL2_Shutdown();
#endif
        imData->created = false;
    }

    OpenGlWidgetWithBrowserPreview::onContextDestroy(e);
}

void ImGuiWidget::setAsCurrentContext()
{
    ImGui::SetCurrentContext(imData->context);
}

void ImGuiWidget::setUseMonospaceFont(const bool useMonoFont)
{
    DISTRHO_SAFE_ASSERT_RETURN(!imData->fontGenerated,);

    imData->useMonospacedFont = useMonoFont;
}

void ImGuiWidget::onHover(const HoverEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MousePos.x = e.pos.x + e.mouseDelta.x;
    io.MousePos.y = e.pos.y + e.mouseDelta.y;

    if (d_isNotEqual(imData->scaleFactor, 1.0f))
    {
        io.MousePos.x *= imData->scaleFactor;
        io.MousePos.y *= imData->scaleFactor;
    }
}

void ImGuiWidget::onDragHover(const DragHoverEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MousePos.x = e.pos.x + e.mouseDelta.x;
    io.MousePos.y = e.pos.y + e.mouseDelta.y;

    if (d_isNotEqual(imData->scaleFactor, 1.0f))
    {
        io.MousePos.x *= imData->scaleFactor;
        io.MousePos.y *= imData->scaleFactor;
    }
}

void ImGuiWidget::onDragEnd(const DragEndEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;
}

void ImGuiWidget::onHoverScroll(const HoverScrollEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    float deltaX = e.scrollDelta.x;
    float deltaY = e.scrollDelta.y;

    if (d_isNotEqual(imData->scaleFactor, 1.0f))
    {
        deltaX *= imData->scaleFactor;
        deltaY *= imData->scaleFactor;
    }

    ImGuiIO& io(ImGui::GetIO());
    io.MouseWheel += deltaY * 0.01f;
    io.MouseWheelH += deltaX * 0.01f;

    if (io.WantCaptureMouse)
        e.consume(this);
}

void ImGuiWidget::onButton(const ButtonEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    switch (e.button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        io.MouseDown[0] = e.action == GLFW_PRESS;
        break;
    /* Don't capture these, let Cardinal handle it instead
    case GLFW_MOUSE_BUTTON_MIDDLE:
        io.MouseDown[1] = e.action == GLFW_PRESS;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        io.MouseDown[2] = e.action == GLFW_PRESS;
        break;
    */
    default:
        return;
    }

    io.KeyCtrl  = e.mods & GLFW_MOD_CONTROL;
    io.KeyShift = e.mods & GLFW_MOD_SHIFT;
    io.KeyAlt   = e.mods & GLFW_MOD_ALT;
    io.KeySuper = e.mods & GLFW_MOD_SUPER;

    if (io.WantCaptureMouse)
        e.consume(this);
}

void ImGuiWidget::onSelectKey(const SelectKeyEvent& e)
{
    if (e.key < 0 || e.key >= IM_ARRAYSIZE(ImGuiIO::KeysDown))
        return;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    switch (e.action)
    {
    case GLFW_PRESS:
        io.KeysDown[e.key] = true;
        break;
    case GLFW_RELEASE:
        io.KeysDown[e.key] = false;
        break;
    default:
        return;
    }

    io.KeyCtrl  = e.mods & GLFW_MOD_CONTROL;
    io.KeyShift = e.mods & GLFW_MOD_SHIFT;
    io.KeyAlt   = e.mods & GLFW_MOD_ALT;
    io.KeySuper = e.mods & GLFW_MOD_SUPER;

    if (io.WantCaptureKeyboard)
        e.consume(this);
}

void ImGuiWidget::onSelectText(const SelectTextEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.AddInputCharacter(e.codepoint);

    if (io.WantCaptureKeyboard)
        e.consume(this);
}

void ImGuiWidget::step()
{
    if (imData->darkMode != settings::darkMode)
    {
        imData->darkMode = settings::darkMode;
        imData->resetEverything(true);
    }

    OpenGlWidgetWithBrowserPreview::step();
}

void ImGuiWidget::drawFramebuffer()
{
    const float scaleFactor = APP->window->pixelRatio * std::max(1.0f, APP->scene->rack->getAbsoluteZoom());

    if (d_isNotEqual(imData->scaleFactor, scaleFactor))
        imData->resetEverything(true);

    drawFramebufferCommon(getFramebufferSize(), scaleFactor);
}

void ImGuiWidget::drawFramebufferForBrowserPreview()
{
    imData->resetEverything(true);
    drawFramebufferCommon(box.size.mult(oversample), oversample);
}

void ImGuiWidget::drawFramebufferCommon(const Vec& fbSize, const float scaleFactor)
{
    ImGui::SetCurrentContext(imData->context);
    ImGuiIO& io(ImGui::GetIO());

    if (d_isNotEqual(imData->scaleFactor, scaleFactor))
    {
        imData->scaleFactor = scaleFactor;

        ImGuiStyle& style(ImGui::GetStyle());
        new(&style)ImGuiStyle();
        imData->resetStyle();

        if (! imData->fontGenerated)
        {
            imData->originalScaleFactor = scaleFactor;
            imData->generateFontIfNeeded();
        }
        else
        {
            io.FontGlobalScale = scaleFactor / imData->originalScaleFactor;
        }
    }

#if defined(DGL_USE_OPENGL3)
    // TODO?
#else
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, box.size.x * scaleFactor, box.size.y * scaleFactor, 0.0, -1.0, 1.0);
    glViewport(0.0, 0.0, fbSize.x, fbSize.y);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    io.DisplaySize = ImVec2(box.size.x * scaleFactor, box.size.y * scaleFactor);
    io.DisplayFramebufferScale = ImVec2(fbSize.x / (box.size.x * scaleFactor), fbSize.y / (box.size.y * scaleFactor));

    if (!imData->created)
    {
#if defined(DGL_USE_OPENGL3)
        ImGui_ImplOpenGL3_Init();
#else
        ImGui_ImplOpenGL2_Init();
#endif
        imData->created = true;
    }

    const double time = glfwGetTime();
    io.DeltaTime = time - imData->lastFrameTime;
    imData->lastFrameTime = time;

#if defined(DGL_USE_OPENGL3)
    ImGui_ImplOpenGL3_NewFrame();
#else
    ImGui_ImplOpenGL2_NewFrame();
#endif

    ImGui::NewFrame();
    drawImGui();
    ImGui::Render();

    if (ImDrawData* const data = ImGui::GetDrawData())
    {
#if defined(DGL_USE_OPENGL3)
        ImGui_ImplOpenGL3_RenderDrawData(data);
#else
        ImGui_ImplOpenGL2_RenderDrawData(data);
#endif
    }
}
