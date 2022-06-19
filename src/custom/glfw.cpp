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

#include "Application.hpp"
#include "PluginContext.hpp"

#include <GLFW/glfw3.h>

GLFWAPI int glfwGetKeyScancode(int) { return 0; }

GLFWAPI const char* glfwGetClipboardString(GLFWwindow*)
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, nullptr);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr, nullptr);

    size_t dataSize;
    return static_cast<const char*>(context->ui->getClipboard(dataSize));
}

GLFWAPI void glfwSetClipboardString(GLFWwindow*, const char* const text)
{
    DISTRHO_SAFE_ASSERT_RETURN(text != nullptr,);

    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr,);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr,);

    context->ui->setClipboard(nullptr, text, std::strlen(text)+1);
}

GLFWAPI void glfwSetCursor(GLFWwindow*, GLFWcursor* const cursor)
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr,);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr,);

    context->ui->setCursor(cursor != nullptr ? kMouseCursorDiagonal : kMouseCursorArrow);
}

GLFWAPI double glfwGetTime(void)
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, 0.0);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr, 0.0);

    return context->ui->getApp().getTime();
}

GLFWAPI const char* glfwGetKeyName(const int key, int)
{
    switch (key)
    {
    case '\"': return "\"";
    case '\'': return "\'";
    case '\\': return "\\";
    case ' ': return " ";
    case '!': return "!";
    case '#': return "#";
    case '$': return "$";
    case '%': return "%";
    case '&': return "&";
    case '(': return "(";
    case ')': return ")";
    case '*': return "*";
    case '+': return "+";
    case ',': return ",";
    case '-': return "-";
    case '.': return ".";
    case '/': return "/";
    case '0': return "0";
    case '1': return "1";
    case '2': return "2";
    case '3': return "3";
    case '4': return "4";
    case '5': return "5";
    case '6': return "6";
    case '7': return "7";
    case '8': return "8";
    case '9': return "9";
    case ':': return ":";
    case ';': return ";";
    case '<': return "<";
    case '=': return "=";
    case '>': return ">";
    case '?': return "?";
    case '@': return "@";
    /* Rack expects lowercase, forced below
    case 'A': return "A";
    case 'B': return "B";
    case 'C': return "C";
    case 'D': return "D";
    case 'E': return "E";
    case 'F': return "F";
    case 'G': return "G";
    case 'H': return "H";
    case 'I': return "I";
    case 'J': return "J";
    case 'K': return "K";
    case 'L': return "L";
    case 'M': return "M";
    case 'N': return "N";
    case 'O': return "O";
    case 'P': return "P";
    case 'Q': return "Q";
    case 'R': return "R";
    case 'S': return "S";
    case 'T': return "T";
    case 'U': return "U";
    case 'V': return "V";
    case 'W': return "W";
    case 'X': return "X";
    case 'Y': return "Y";
    case 'Z': return "Z";
    */
    case '[': return "[";
    case ']': return "]";
    case '^': return "^";
    case '_': return "_";
    case '`': return "`";
    case 'a': case 'A': return "a";
    case 'b': case 'B': return "b";
    case 'c': case 'C': return "c";
    case 'd': case 'D': return "d";
    case 'e': case 'E': return "e";
    case 'f': case 'F': return "f";
    case 'g': case 'G': return "g";
    case 'h': case 'H': return "h";
    case 'i': case 'I': return "i";
    case 'j': case 'J': return "j";
    case 'k': case 'K': return "k";
    case 'l': case 'L': return "l";
    case 'm': case 'M': return "m";
    case 'n': case 'N': return "n";
    case 'o': case 'O': return "o";
    case 'p': case 'P': return "p";
    case 'q': case 'Q': return "q";
    case 'r': case 'R': return "r";
    case 's': case 'S': return "s";
    case 't': case 'T': return "t";
    case 'u': case 'U': return "u";
    case 'v': case 'V': return "v";
    case 'w': case 'W': return "w";
    case 'x': case 'X': return "x";
    case 'y': case 'Y': return "y";
    case 'z': case 'Z': return "z";
    default: return nullptr;
    }
}
