/*
 * Dear ImGui for DPF, converted to VCV
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
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

#if defined(DGL_USE_GLES2)
# define IMGUI_IMPL_OPENGL_ES2
#elif defined(DGL_USE_GLES3)
# define IMGUI_IMPL_OPENGL_ES3
#elif defined(DGL_USE_OPENGL3)
# define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#define IMGUI_DPF_BACKEND
#include "DearImGui/imgui.cpp"
#include "DearImGui/imgui_demo.cpp"
#include "DearImGui/imgui_draw.cpp"
#include "DearImGui/imgui_tables.cpp"
#include "DearImGui/imgui_widgets.cpp"
#if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
# include "DearImGui/imgui_impl_opengl3.cpp"
#else
# include "DearImGui/imgui_impl_opengl2.cpp"
#endif
