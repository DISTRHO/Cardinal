/*
 * DISTRHO glBars Plugin based on XMMS/XBMC "GL Bars"
 * Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 * Copyright (C) 2000 Christian Zander <phoenix@minion.de>
 * Copyright (C) 2015 Nedko Arnaudov
 * Copyright (C) 2016-2019 Filipe Coelho <falktx@falktx.com>
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
void draw_bar(GLenum mode, GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue)
{
    const GLfloat width = 0.1;

    if (mode == GL_POINT)
        glColor3f(0.2, 1.0, 0.2);

    if (mode != GL_POINT)
    {
        glColor3f(red,green,blue);
        draw_rectangle(x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
    }
    draw_rectangle(x_offset, 0, z_offset, x_offset + width, 0, z_offset + 0.1);

    if (mode != GL_POINT)
    {
        glColor3f(0.5 * red, 0.5 * green, 0.5 * blue);
        draw_rectangle(x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);
    }
    draw_rectangle(x_offset, 0.0, z_offset, x_offset + width, height, z_offset );

    if (mode != GL_POINT)
    {
        glColor3f(0.25 * red, 0.25 * green, 0.25 * blue);
        draw_rectangle(x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);
    }
    draw_rectangle(x_offset + width, 0.0, z_offset , x_offset + width, height, z_offset + 0.1);
}

struct glBarsState {
    GLenum g_mode;
    GLfloat x_angle, x_speed;
    GLfloat y_angle, y_speed;
    GLfloat z_angle, z_speed;
    GLfloat heights[16][16], cHeights[16][16], scale;
    GLfloat hSpeed;

    glBarsState()
    {
        g_mode = GL_FILL;
        x_angle = 20.0;
        x_speed = 0.0;
        y_angle = 15.0; // was 45
        y_speed = 0.5;
        z_angle = 0.0;
        z_speed = 0.0;

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

        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
                cHeights[y][x] = heights[y][x] = 0;
        }
    }

    void drawBars()
    {
        GLfloat x_offset, z_offset, r_base, b_base;

        glClear(GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        glTranslatef(0.0,-0.5,-5.0);
        glRotatef(x_angle,1.0,0.0,0.0);
        glRotatef(y_angle,0.0,1.0,0.0);
        glRotatef(z_angle,0.0,0.0,1.0);

        glPolygonMode(GL_FRONT_AND_BACK, g_mode);
        glBegin(GL_TRIANGLES);

        for (int y = 0; y < 16; y++)
        {
            z_offset = -1.6 + ((15 - y) * 0.2);

            b_base = y * (1.0 / 15);
            r_base = 1.0 - b_base;

            for (int x = 0; x < 16; x++)
            {
                x_offset = -1.6 + ((float)x * 0.2);
                if (::fabs(cHeights[y][x]-heights[y][x])>hSpeed)
                {
                  if (cHeights[y][x]<heights[y][x])
                      cHeights[y][x] += hSpeed;
                  else
                      cHeights[y][x] -= hSpeed;
                }
                draw_bar(g_mode, x_offset, z_offset,
                         cHeights[y][x], r_base - (float(x) * (r_base / 15.0)),
                         (float)x * (1.0 / 15), b_base /*, 16*y+x*/);
            }
        }

        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPopMatrix();
    }

    void Render()
    {
        glDisable(GL_BLEND);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glFrustum(-1, 1, -1, 1, 1.5, 10);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glPolygonMode(GL_FRONT, GL_FILL);
        //glPolygonMode(GL_BACK, GL_FILL);

        x_angle += x_speed;
        if (x_angle >= 360.0)
            x_angle -= 360.0;

        y_angle += y_speed;
        if (y_angle >= 360.0)
            y_angle -= 360.0;

        z_angle += z_speed;
        if (z_angle >= 360.0)
            z_angle -= 360.0;

        drawBars();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
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
