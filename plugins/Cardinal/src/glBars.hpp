/*
 * DISTRHO glBars Plugin based on XMMS/XBMC "GL Bars"
 * Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 * Copyright (C) 2000 Christian Zander <phoenix@minion.de>
 * Copyright (C) 2015 Nedko Arnaudov
 * Copyright (C) 2016-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * For a full copy of the license see the LICENSE file.
 */

#ifndef GLBARS_STATE_HPP_INCLUDED
#define GLBARS_STATE_HPP_INCLUDED

#include "plugin.hpp"

static inline
void draw_rectangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
    if (y1 == y2)
    {
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y1, z1);
        glVertex3f(x2, y2, z2);

        glVertex3f(x2, y2, z2);
        glVertex3f(x1, y2, z2);
        glVertex3f(x1, y1, z1);
    }
    else
    {
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y1, z2);
        glVertex3f(x2, y2, z2);

        glVertex3f(x2, y2, z2);
        glVertex3f(x1, y2, z1);
        glVertex3f(x1, y1, z1);
    }
}

static inline
void draw_bar(GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue)
{
    static constexpr const GLfloat width = 0.1;

    // left
    glColor3f(0.25 * red, 0.25 * green, 0.25 * blue);
    draw_rectangle(x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);

    // right
    glColor3f(0.5 * red, 0.5 * green, 0.5 * blue);
    draw_rectangle(x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);

    // top
    glColor3f(red, green, blue);
    draw_rectangle(x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
}

struct glBarsState {
    GLfloat heights[16][16], cHeights[16][16], scale;
    GLfloat hSpeed;

    glBarsState()
    {
        // Set "Bar Height"
        scale = 1.f   / log(256.f); // "Default" / standard
        //scale = 2.f   / log(256.f); // "Big"
        //scale = 3.f   / log(256.f); // "Very Big" / real big
        //scale = 0.33f / log(256.f); // unused
        //scale = 0.5f  / log(256.f); // "Small"

        // Set "Speed"
        //hSpeed = 0.025f;          // "Slow"
        hSpeed = 0.0125f;         // "Default"
        //hSpeed = 0.1f;            // "Fast"
        //hSpeed = 0.2f;            // "Very Fast"
        //hSpeed = 0.05f;           // "Very Slow"

        std::memset(heights, 0, sizeof(heights));
        std::memset(cHeights, 0, sizeof(cHeights));
    }

    void Render()
    {
        GLfloat x_offset, z_offset, r_base, b_base;

        glPushMatrix();
        glTranslatef(0.0,0.25,-4.0);
        glRotatef(30.0,1.0,0.0,0.0);
        glRotatef(45,0.0,1.0,0.0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);

        for (int y = 16; --y >= 0;)
        {
            z_offset = -1.6 + ((15 - y) * 0.2);

            b_base = y * (1.0 / 15);
            r_base = 1.0 - b_base;

            for (int x = 16; --x >= 0;)
            {
                x_offset = -1.6 + ((float)x * 0.2);
                if (::fabs(cHeights[y][x]-heights[y][x])>hSpeed)
                {
                  if (cHeights[y][x]<heights[y][x])
                      cHeights[y][x] += hSpeed;
                  else
                      cHeights[y][x] -= hSpeed;
                }

                draw_bar(x_offset, z_offset,
                         cHeights[y][x], r_base - (float(x) * (r_base / 15.0)),
                         (float)x * (1.0 / 15), b_base /*, 16*y+x*/);
            }
        }

        glEnd();
        glPopMatrix();
    }

    void AudioData(const float* pAudioData, int iAudioDataLength)
    {
        const int xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};

        GLfloat val;

        for (int y = 15; y > 0; y--)
        {
            for (int i = 0; i < 16; i++)
                heights[y][i] = heights[y - 1][i];
        }

        for (int i = 0; i < 16; i++)
        {
            int y = 0;
            for (int c = xscale[i]; c < xscale[i + 1]; c++)
            {
                if (c<iAudioDataLength)
                {
                    if ((int)(pAudioData[c] * (INT16_MAX)) > y)
                        y = (int)(pAudioData[c] * (INT16_MAX));
                }
                else
                {
                    continue;
                }
            }
            y >>= 7;
            if (y > 0)
                val = (logf(y) * scale);
            else
                val = 0;
            heights[0][i] = val;
        }
    }
};

#endif // GLBARS_STATE_HPP_INCLUDED
