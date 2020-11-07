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
#include "i_video.h"

static GLuint wipe_scr_start_tex = 0;
static GLuint wipe_scr_end_tex = 0;
#ifdef __vita__
static GLuint wipe_scr_end_fb = 0;
static unsigned char *scr_buffer = NULL;
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
  if (!scr_buffer) scr_buffer = malloc(3 * 960 * 544);
  if (!scr_buffer) return 0;
  glReadPixels(0, 0, SCREENWIDTH, SCREENHEIGHT, GL_RGB, GL_UNSIGNED_BYTE, scr_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, scr_buffer);
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
  float dU, x0, y0, u0;

  total_w = gld_GetTexDimension(SCREENWIDTH);
  total_h = gld_GetTexDimension(SCREENHEIGHT);

  fU1 = 0.0f;
  fU2 = (float)SCREENWIDTH / (float)total_w;
  fV1 = (float)SCREENHEIGHT / (float)total_h;
  fV2 = 0.0f;
  dU = 1.f / (float)total_w;
  
  gld_EnableTexture2D(GL_TEXTURE0_ARB, true);
  
  glBindTexture(GL_TEXTURE_2D, wipe_scr_end_tex);
  gld_glColor3f(1.0f, 1.0f, 1.0f);

  gld_glBegin(GL_TRIANGLE_STRIP);
  {
#ifdef __vita__
    // framebuffer texture is flipped
    gld_glTexCoord2f(fU1, fV2); gld_glVertex2f(0.0f, 0.0f);
    gld_glTexCoord2f(fU1, fV1); gld_glVertex2f(0.0f, (float)SCREENHEIGHT);
    gld_glTexCoord2f(fU2, fV2); gld_glVertex2f((float)SCREENWIDTH, 0.0f);
    gld_glTexCoord2f(fU2, fV1); gld_glVertex2f((float)SCREENWIDTH, (float)SCREENHEIGHT);
#else
    gld_glTexCoord2f(fU1, fV1); gld_glVertex2f(0.0f, 0.0f);
    gld_glTexCoord2f(fU1, fV2); gld_glVertex2f(0.0f, (float)SCREENHEIGHT);
    gld_glTexCoord2f(fU2, fV1); gld_glVertex2f((float)SCREENWIDTH, 0.0f);
    gld_glTexCoord2f(fU2, fV2); gld_glVertex2f((float)SCREENWIDTH, (float)SCREENHEIGHT);
#endif
  }
  gld_glEnd();
  
  glBindTexture(GL_TEXTURE_2D, wipe_scr_start_tex);
  gld_glColor3f(1.0f, 1.0f, 1.0f);

  gld_glBegin(GL_TRIANGLE_STRIP);
  for (i = 0; i < SCREENWIDTH; ++i)
  {
    x0 = (float) i;
    y0 = (float) y_lookup[i];
    u0 = fU1 + dU * i;
    gld_glTexCoord2f(u0, fV1); gld_glVertex2f(x0, y0);
    gld_glTexCoord2f(u0, fV2); gld_glVertex2f(x0, y0 + (float)SCREENHEIGHT);
  }
  gld_glEnd();
  
  return 0;
}

int gld_wipe_exitMelt(int ticks)
{
  glFinish();

  if (wipe_scr_start_tex != 0)
  {
    glDeleteTextures(1, &wipe_scr_start_tex);
    wipe_scr_start_tex = 0;
  }
#ifdef __vita__
  if (wipe_scr_end_fb != 0)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &wipe_scr_end_fb);
    wipe_scr_end_fb = 0;
  }
#endif
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

#ifdef __vita__
  // switch render target to a framebuffer so we can use that as the end texture later
  glGenTextures(1, &wipe_scr_end_tex);
  glBindTexture(GL_TEXTURE_2D, wipe_scr_end_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, scr_buffer);
  glGenFramebuffers(1, &wipe_scr_end_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, wipe_scr_end_fb);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, wipe_scr_end_tex, 0);
  vglStartRendering();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  vglStopRendering();
  glFinish();
#endif

  return 0;
}

int gld_wipe_EndScreen(void)
{
  I_StopRendering(1);

#ifdef __vita__
  // we're done rendering to a framebuffer, unbind it
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
  wipe_scr_end_tex = CaptureScreenAsTexID();
#endif

  return 0;
}
