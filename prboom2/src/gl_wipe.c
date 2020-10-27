/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *
 *---------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gl_opengl.h"

#include "v_video.h"
#include "gl_intern.h"
#include "m_random.h"
#include "lprintf.h"
#include "e6y.h"

static GLuint wipe_scr_start_tex = 0;
static GLuint wipe_scr_end_tex = 0;
#ifdef __vita__
static unsigned char *scr_buffer = NULL;
static GLuint scr_size = 0;
#endif

GLuint CaptureScreenAsTexID(void)
{
  GLuint id;

  gld_EnableTexture2D(GL_TEXTURE0_ARB, true);
 
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

#ifdef __vita__
  if (scr_size != SCREENWIDTH * SCREENHEIGHT)
  {
    free(scr_buffer);
    scr_size = SCREENWIDTH * SCREENHEIGHT;
    scr_buffer = malloc(scr_size * 4);
  }
  if (scr_buffer)
  {
    glReadPixels(0, 0, SCREENWIDTH, SCREENHEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, scr_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
      gld_GetTexDimension(SCREENWIDTH), gld_GetTexDimension(SCREENHEIGHT), 
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  }
#else
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 
    gld_GetTexDimension(SCREENWIDTH), gld_GetTexDimension(SCREENHEIGHT), 
    0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, SCREENWIDTH, SCREENHEIGHT);
#endif

  return id;
}

int gld_wipe_doMelt(int ticks, int *y_lookup)
{
  int i;
  int total_w, total_h;
  float fU1, fU2, fV1, fV2;
  float sx_prev, sy_prev, tx_prev;

  total_w = gld_GetTexDimension(SCREENWIDTH);
  total_h = gld_GetTexDimension(SCREENHEIGHT);

  fU1 = 0.0f;
  fV1 = (float)SCREENHEIGHT / (float)total_h;
  fU2 = (float)SCREENWIDTH / (float)total_w;
  fV2 = 0.0f;
  
  gld_EnableTexture2D(GL_TEXTURE0_ARB, true);
  
  glBindTexture(GL_TEXTURE_2D, wipe_scr_end_tex);
  gld_glColor3f(1.0f, 1.0f, 1.0f);

  gld_glBegin(GL_TRIANGLE_STRIP);
  {
    gld_glTexCoord2f(fU1, fV1); gld_glVertex2f(0.0f, 0.0f);
    gld_glTexCoord2f(fU1, fV2); gld_glVertex2f(0.0f, (float)SCREENHEIGHT);
    gld_glTexCoord2f(fU2, fV1); gld_glVertex2f((float)SCREENWIDTH, 0.0f);
    gld_glTexCoord2f(fU2, fV2); gld_glVertex2f((float)SCREENWIDTH, (float)SCREENHEIGHT);
  }
  gld_glEnd();
  
  glBindTexture(GL_TEXTURE_2D, wipe_scr_start_tex);
  gld_glColor3f(1.0f, 1.0f, 1.0f);

  sx_prev = 0.f;
  sy_prev = MAX(0, y_lookup[0]);
  tx_prev = 0.f;

  gld_glBegin(GL_QUADS);

  for (i=1; i <= SCREENWIDTH; i++)
  {
    int yoffs = MAX(0, y_lookup[i]);
    
    float tx = (float) i / total_w;
    float sx = (float) i;
    float sy = (float) yoffs;

    gld_glTexCoord2f(tx_prev, fV1); gld_glVertex2f(sx_prev, sy_prev);
    gld_glTexCoord2f(tx_prev, fV2); gld_glVertex2f(sx_prev, sy_prev + (float)SCREENHEIGHT);
    gld_glTexCoord2f(tx,      fV2); gld_glVertex2f(sx,      sy      + (float)SCREENHEIGHT);
    gld_glTexCoord2f(tx,      fV1); gld_glVertex2f(sx,      sy);

    sx_prev = sx; sy_prev = sy;
    tx_prev = tx;
  }
  
  gld_glEnd();
  
  return 0;
}

int gld_wipe_exitMelt(int ticks)
{
  if (wipe_scr_start_tex != 0)
  {
    glDeleteTextures(1, &wipe_scr_start_tex);
    wipe_scr_start_tex = 0;
  }
  if (wipe_scr_end_tex != 0)
  {
    glDeleteTextures(1, &wipe_scr_end_tex);
    wipe_scr_end_tex = 0;
  }

  gld_ResetLastTexture();

  return 0;
}

int gld_wipe_StartScreen(void)
{
  wipe_scr_start_tex = CaptureScreenAsTexID();

  return 0;
}

int gld_wipe_EndScreen(void)
{
  glFlush();
  wipe_scr_end_tex = CaptureScreenAsTexID();

  return 0;
}
