/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2006 by
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
 *  DOOM graphics stuff for SDL
 *
 *-----------------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif // _WIN32

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "SDL.h"
//e6y
#ifdef _WIN32
#include <SDL_syswm.h>
#endif

#ifdef __vita__
#include <vitaGL/source/vitaGL.h>
#endif

#include "m_argv.h"
#include "doomstat.h"
#include "doomdef.h"
#include "doomtype.h"
#include "v_video.h"
#include "r_draw.h"
#include "r_things.h"
#include "r_plane.h"
#include "r_main.h"
#include "f_wipe.h"
#include "d_main.h"
#include "d_event.h"
#include "d_deh.h"
#include "i_joy.h"
#include "i_video.h"
#include "i_capture.h"
#include "z_zone.h"
#include "s_sound.h"
#include "sounds.h"
#include "w_wad.h"
#include "st_stuff.h"
#include "am_map.h"
#include "g_game.h"
#include "lprintf.h"

#ifdef GL_DOOM
#include "gl_struct.h"
#endif

#include "e6y.h"//e6y
#include "i_main.h"

//e6y: new mouse code
static SDL_Cursor* cursors[2] = {NULL, NULL};

dboolean window_focused;
int mouse_currently_grabbed = true;

// Window resize state.
static void ApplyWindowResize(SDL_Event *resize_event);

const char *sdl_video_window_pos;

static void ActivateMouse(void);
static void DeactivateMouse(void);
//static int AccelerateMouse(int val);
static void I_ReadMouse(void);
static dboolean MouseShouldBeGrabbed();
static void UpdateFocus(void);

int gl_colorbuffer_bits=16;
int gl_depthbuffer_bits=16;

extern void M_QuitDOOM(int choice);
int use_fullscreen;
int desired_fullscreen;
int render_vsync;
int screen_multiply;
int render_screen_multiply;
SDL_Surface *screen;
SDL_Surface *surface;
SDL_Surface *buffer;
SDL_Window *sdl_window;
SDL_Renderer *sdl_renderer;
unsigned int windowid = 0;
SDL_Rect src_rect = { 0, 0, 0, 0 };
SDL_Rect dst_rect = { 0, 0, 0, 0 };

#ifdef __vita__
static GLuint sw_texture;
static GLenum sw_texfmt;
static GLenum sw_textype;
static GLuint sw_texsize;
static void *sw_texptr;
#define DEFAULT_SCREEN_W 960
#define DEFAULT_SCREEN_H 544
#else
SDL_Texture *sdl_texture;
SDL_Texture *sdl_texture_upscaled;
SDL_GLContext sdl_glcontext;
#define DEFAULT_SCREEN_W 640
#define DEFAULT_SCREEN_H 480
#endif

////////////////////////////////////////////////////////////////////////////
// Input code
int             leds_always_off = 0; // Expected by m_misc, not relevant

// Mouse handling
extern int     usemouse;        // config file var
static dboolean mouse_enabled; // usemouse, but can be overriden by -nomouse

int I_GetModeFromString(const char *modestr);

/////////////////////////////////////////////////////////////////////////////////
// Keyboard handling

//
//  Translates the key currently in key
//

static int I_TranslateKey(SDL_Keysym* key)
{
  int rc = 0;

  switch (key->sym) {
  case SDLK_LEFT: rc = KEYD_LEFTARROW;  break;
  case SDLK_RIGHT:  rc = KEYD_RIGHTARROW; break;
  case SDLK_DOWN: rc = KEYD_DOWNARROW;  break;
  case SDLK_UP:   rc = KEYD_UPARROW;  break;
  case SDLK_ESCAPE: rc = KEYD_ESCAPE; break;
  case SDLK_RETURN: rc = KEYD_ENTER;  break;
  case SDLK_TAB:  rc = KEYD_TAB;    break;
  case SDLK_F1:   rc = KEYD_F1;   break;
  case SDLK_F2:   rc = KEYD_F2;   break;
  case SDLK_F3:   rc = KEYD_F3;   break;
  case SDLK_F4:   rc = KEYD_F4;   break;
  case SDLK_F5:   rc = KEYD_F5;   break;
  case SDLK_F6:   rc = KEYD_F6;   break;
  case SDLK_F7:   rc = KEYD_F7;   break;
  case SDLK_F8:   rc = KEYD_F8;   break;
  case SDLK_F9:   rc = KEYD_F9;   break;
  case SDLK_F10:  rc = KEYD_F10;    break;
  case SDLK_F11:  rc = KEYD_F11;    break;
  case SDLK_F12:  rc = KEYD_F12;    break;
  case SDLK_BACKSPACE:  rc = KEYD_BACKSPACE;  break;
  case SDLK_DELETE: rc = KEYD_DEL;  break;
  case SDLK_INSERT: rc = KEYD_INSERT; break;
  case SDLK_PAGEUP: rc = KEYD_PAGEUP; break;
  case SDLK_PAGEDOWN: rc = KEYD_PAGEDOWN; break;
  case SDLK_HOME: rc = KEYD_HOME; break;
  case SDLK_END:  rc = KEYD_END;  break;
  case SDLK_PAUSE:  rc = KEYD_PAUSE;  break;
  case SDLK_EQUALS: rc = KEYD_EQUALS; break;
  case SDLK_MINUS:  rc = KEYD_MINUS;  break;
  case SDLK_KP_0:  rc = KEYD_KEYPAD0;  break;
  case SDLK_KP_1:  rc = KEYD_KEYPAD1;  break;
  case SDLK_KP_2:  rc = KEYD_KEYPAD2;  break;
  case SDLK_KP_3:  rc = KEYD_KEYPAD3;  break;
  case SDLK_KP_4:  rc = KEYD_KEYPAD4;  break;
  case SDLK_KP_5:  rc = KEYD_KEYPAD5;  break;
  case SDLK_KP_6:  rc = KEYD_KEYPAD6;  break;
  case SDLK_KP_7:  rc = KEYD_KEYPAD7;  break;
  case SDLK_KP_8:  rc = KEYD_KEYPAD8;  break;
  case SDLK_KP_9:  rc = KEYD_KEYPAD9;  break;
  case SDLK_KP_PLUS:  rc = KEYD_KEYPADPLUS; break;
  case SDLK_KP_MINUS: rc = KEYD_KEYPADMINUS;  break;
  case SDLK_KP_DIVIDE:  rc = KEYD_KEYPADDIVIDE; break;
  case SDLK_KP_MULTIPLY: rc = KEYD_KEYPADMULTIPLY; break;
  case SDLK_KP_ENTER: rc = KEYD_KEYPADENTER;  break;
  case SDLK_KP_PERIOD:  rc = KEYD_KEYPADPERIOD; break;
  case SDLK_LSHIFT:
  case SDLK_RSHIFT: rc = KEYD_RSHIFT; break;
  case SDLK_LCTRL:
  case SDLK_RCTRL:  rc = KEYD_RCTRL;  break;
  case SDLK_LALT:
  case SDLK_LGUI:
  case SDLK_RALT:
  case SDLK_RGUI:  rc = KEYD_RALT;   break;
  case SDLK_CAPSLOCK: rc = KEYD_CAPSLOCK; break;
  case SDLK_PRINTSCREEN: rc = KEYD_PRINTSC; break;
  default:    rc = key->sym;    break;
  }

  return rc;

}

/////////////////////////////////////////////////////////////////////////////////
// Main input code

static void I_GetEvent(void)
{
  event_t event;

  SDL_Event SDLEvent;
  SDL_Event *Event = &SDLEvent;

  static int mwheeluptic = 0, mwheeldowntic = 0;

while (SDL_PollEvent(Event))
{
  switch (Event->type) {
  case SDL_KEYDOWN:
#ifdef MACOSX
    if (Event->key.keysym.mod & KMOD_META)
    {
      // Switch windowed<->fullscreen if pressed <Command-F>
      if (Event->key.keysym.sym == SDLK_f)
      {
        V_ToggleFullscreen();
        break;
      }
    }
#else
    if (Event->key.keysym.mod & KMOD_LALT)
    {
      // Prevent executing action on Alt-Tab
      if (Event->key.keysym.sym == SDLK_TAB)
      {
        break;
      }
      // Switch windowed<->fullscreen if pressed Alt-Enter
      else if (Event->key.keysym.sym == SDLK_RETURN)
      {
        V_ToggleFullscreen();
        break;
      }
      // Immediately exit on Alt+F4 ("Boss Key")
      else if (Event->key.keysym.sym == SDLK_F4)
      {
        I_SafeExit(0);
        break;
      }
    }
#endif
    event.type = ev_keydown;
    event.data1 = I_TranslateKey(&Event->key.keysym);
    D_PostEvent(&event);
    break;

  case SDL_KEYUP:
    {
      event.type = ev_keyup;
      event.data1 = I_TranslateKey(&Event->key.keysym);
      D_PostEvent(&event);
    }
    break;

  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    if (mouse_enabled && window_focused)
    {
      event.type = Event->button.state == SDL_PRESSED ? ev_keydown : ev_keyup;
      event.data1 = Event->button.button - 1 + KEYD_MOUSE1;
      D_PostEvent(&event);
    }
    break;

  case SDL_CONTROLLERBUTTONDOWN:
  case SDL_CONTROLLERBUTTONUP:
    event.type = Event->cbutton.state == SDL_PRESSED ? ev_keydown : ev_keyup;
    event.data1 = Event->cbutton.button + KEYD_JOY_BASE;
    D_PostEvent(&event);
    break;

  case SDL_MOUSEWHEEL:
    if (mouse_enabled && window_focused)
    {
      if (Event->wheel.y > 0)
      {
        event.type = ev_keydown;
        event.data1 = KEYD_MWHEELUP;
        mwheeldowntic = gametic;
        D_PostEvent(&event);
      }
      else if (Event->wheel.y < 0)
      {
        event.type = ev_keydown;
        event.data1 = KEYD_MWHEELDOWN;
        mwheeluptic = gametic;
        D_PostEvent(&event);
      }
    }
    break;

  case SDL_MOUSEMOTION:
    if (mouse_enabled && window_focused)
    {
      event.type = ev_mouse;
      event.data1 = 0; // fgs: mouse buttons now virtual keys
      event.data2 = Event->motion.xrel << 4;
      event.data3 = -Event->motion.yrel << 4;
      D_PostEvent(&event);
    }
    break;

  case SDL_WINDOWEVENT:
    if (Event->window.windowID == windowid)
    {
      switch (Event->window.event)
      {
      case SDL_WINDOWEVENT_FOCUS_GAINED:
      case SDL_WINDOWEVENT_FOCUS_LOST:
        UpdateFocus();
        break;
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        ApplyWindowResize(Event);
        break;
      }
    }
    break;

  case SDL_QUIT:
    S_StartSound(NULL, sfx_swtchn);
    M_QuitDOOM(0);

  default:
    break;
  }
}

  if(mwheeluptic && mwheeluptic + 1 < gametic)
  {
    event.type = ev_keyup;
    event.data1 = KEYD_MWHEELUP;
    D_PostEvent(&event);
    mwheeluptic = 0;
  }

  if(mwheeldowntic && mwheeldowntic + 1 < gametic)
  {
    event.type = ev_keyup;
    event.data1 = KEYD_MWHEELDOWN;
    D_PostEvent(&event);
    mwheeldowntic = 0;
  }
}

//
// Wrappers around SDL resolution getting functions
//

#ifdef __vita__
static const SDL_DisplayMode vita_modes[] =
{
  { SDL_PIXELFORMAT_BGRA8888, 320, 200, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 320, 240, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 480, 272, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 640, 368, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 640, 400, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 640, 480, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 720, 408, 60, NULL },
  { SDL_PIXELFORMAT_BGRA8888, 960, 544, 60, NULL },
};
static const int vita_nummodes = sizeof(vita_modes) / sizeof(*vita_modes);
#endif

static int I_GetNumDisplayModes(const int disp)
{
#ifdef __vita__
  return vita_nummodes;
#else
  return SDL_GetNumDisplayModes(disp);
#endif
}

static int I_GetDisplayMode(const int disp, const int mode, SDL_DisplayMode *out)
{
#ifdef __vita__
  if (mode >= 0 && mode < vita_nummodes)
  {
    *out = vita_modes[mode];
    return 0;
  }
  return -1;
#else
  return SDL_GetDisplayMode(disp, mode, out);
#endif
}

//
// I_StartTic
//

void I_StartTic (void)
{
  I_GetEvent();

  I_ReadMouse();

  I_PollJoystick();
}

//
// I_StartFrame
//
void I_StartFrame (void)
{
}

//
// I_InitInputs
//

static void I_InitInputs(void)
{
  static Uint8 empty_cursor_data = 0;

  int nomouse_parm = M_CheckParm("-nomouse");

  // check if the user wants to use the mouse
  mouse_enabled = usemouse && !nomouse_parm;
  
  SDL_PumpEvents();

  // Save the default cursor so it can be recalled later
  cursors[0] = SDL_GetCursor();
  // Create an empty cursor
  cursors[1] = SDL_CreateCursor(&empty_cursor_data, &empty_cursor_data, 8, 1, 0, 0);

  if (mouse_enabled)
  {
    MouseAccelChanging();
  }

  I_InitJoystick();
}
/////////////////////////////////////////////////////////////////////////////

// I_SkipFrame
//
// Returns true if it thinks we can afford to skip this frame

inline static dboolean I_SkipFrame(void)
{
  static int frameno;

  frameno++;
  switch (gamestate) {
  case GS_LEVEL:
    if (!paused)
      return false;
  default:
    // Skip odd frames
    return (frameno & 1) ? true : false;
  }
}

///////////////////////////////////////////////////////////
// Palette stuff.
//
static void I_UploadNewPalette(int pal, int force)
{
  // This is used to replace the current 256 colour cmap with a new one
  // Used by 256 colour PseudoColor modes

  // Array of SDL_Color structs used for setting the 256-colour palette
  static SDL_Color* colours;
  static int cachedgamma;
  static size_t num_pals;

  if (V_GetMode() != VID_MODE8)
    return;

  if ((colours == NULL) || (cachedgamma != usegamma) || force) {
    int pplump = W_GetNumForName("PLAYPAL");
    int gtlump = (W_CheckNumForName)("GAMMATBL",ns_prboom);
    register const byte * palette = (const byte*)W_CacheLumpNum(pplump);
    register const byte * const gtable = (const byte *)W_CacheLumpNum(gtlump) + 256*(cachedgamma = usegamma);
    register int i;

    num_pals = W_LumpLength(pplump) / (3*256);
    num_pals *= 256;

    if (!colours) {
      // First call - allocate and prepare colour array
      colours = (SDL_Color*)malloc(sizeof(*colours)*num_pals);
    }

    // set the colormap entries
    for (i=0 ; (size_t)i<num_pals ; i++) {
      colours[i].r = gtable[palette[0]];
      colours[i].g = gtable[palette[1]];
      colours[i].b = gtable[palette[2]];
      palette += 3;
    }

    W_UnlockLumpNum(pplump);
    W_UnlockLumpNum(gtlump);
    num_pals/=256;
  }

#ifdef RANGECHECK
  if ((size_t)pal >= num_pals)
    I_Error("I_UploadNewPalette: Palette number out of range (%d>=%d)",
      pal, num_pals);
#endif

  SDL_SetPaletteColors(screen->format->palette, colours+256*pal, 0, 256);
}

//////////////////////////////////////////////////////////////////////////////
// Graphics API

void I_ShutdownGraphics(void)
{
  SDL_FreeCursor(cursors[1]);
  DeactivateMouse();
}

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
}

//
// I_StartRendering
//
static int in_render_frame = 0;
void I_StartRendering (void)
{
  if (!in_render_frame)
  {
    in_render_frame = 1;
#ifdef __vita__
    vglStartRendering();
#endif
  }
}

//
// I_StopRendering
//
void I_StopRendering(int wait)
{
#ifdef __vita__
  if (in_render_frame)
  {
    in_render_frame = 0;
    vglStopRendering();
  }
#endif
#ifdef GL_DOOM
  if (wait)
    glFinish();
#endif
}

//
// I_FinishUpdate
//
static int newpal = 0;
#define NO_PALETTE_CHANGE 1000

void I_FinishUpdate (void)
{
  //e6y: new mouse code
  UpdateGrab();

  // The screen wipe following pressing the exit switch on a level
  // is noticably jerkier with I_SkipFrame
  // if (I_SkipFrame())return;

#ifdef MONITOR_VISIBILITY
  //!!if (!(SDL_GetAppState()&SDL_APPACTIVE)) {
  //!!  return;
  //!!}
#endif

#ifdef GL_DOOM
  if (V_GetMode() == VID_MODEGL) {
    // proff 04/05/2000: swap OpenGL buffers
    gld_Finish();
    I_StopRendering(0);
    return;
  }
#endif

#ifndef __vita__
  if ((screen_multiply > 1) || SDL_MUSTLOCK(screen)) {
      int h;
      byte *src;
      byte *dest;

      if (SDL_LockSurface(screen) < 0) {
        lprintf(LO_INFO,"I_FinishUpdate: %s\n", SDL_GetError());
        return;
      }

      dest=(byte*)screen->pixels;
      src=screens[0].data;
      h=screen->h;
      for (; h>0; h--)
      {
        memcpy(dest,src,SCREENWIDTH*V_GetPixelDepth()); //e6y
        dest+=screen->pitch;
        src+=screens[0].byte_pitch;
      }

      SDL_UnlockSurface(screen);
  }
#endif

  /* Update the display buffer (flipping video pages if supported)
   * If we need to change palette, that implicitely does a flip */
  if (newpal != NO_PALETTE_CHANGE) {
    I_UploadNewPalette(newpal, false);
    newpal = NO_PALETTE_CHANGE;
  }

  // Blit from the paletted 8-bit screen buffer to the intermediate
  // 32-bit RGBA buffer that we can load into the texture.
  if (buffer != screen)
    SDL_LowerBlit(screen, &src_rect, buffer, &src_rect);

#ifdef __vita__
  // don't need to memcpy anything because buffer already points
  // to the internaltexture data

  I_StartRendering(); // in case we're not rendering yet

  // Make sure the pillarboxes are kept clear each frame.
  glClear(GL_COLOR_BUFFER_BIT);

  glColor4f(1.f, 1.f, 1.f, 1.f);

  // Draw our buffer to screen, possibly upscaling it.
  glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.f, 0.f);
    glVertex3f(dst_rect.x, dst_rect.y, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(dst_rect.x + dst_rect.w, dst_rect.y, 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex3f(dst_rect.x + dst_rect.w, dst_rect.y + dst_rect.h, 0.f);
    glTexCoord2f(0.f, 1.f);
    glVertex3f(dst_rect.x, dst_rect.y + dst_rect.h, 0.f);
  glEnd();

  I_StopRendering(0);
#else
  // Update the intermediate texture with the contents of the RGBA buffer.
  SDL_UpdateTexture(sdl_texture, &src_rect, buffer->pixels, buffer->pitch);

  // Make sure the pillarboxes are kept clear each frame.
  SDL_RenderClear(sdl_renderer);

  if (screen_multiply > 1)
  {
    // Render this intermediate texture into the upscaled texture
    // using "nearest" integer scaling.
    SDL_SetRenderTarget(sdl_renderer, sdl_texture_upscaled);
    SDL_RenderCopy(sdl_renderer, sdl_texture, &src_rect, NULL);

    // Finally, render this upscaled texture to screen using linear scaling.
    SDL_SetRenderTarget(sdl_renderer, NULL);
    SDL_RenderCopy(sdl_renderer, sdl_texture_upscaled, NULL, NULL);
  }
  else
  {
    SDL_RenderCopy(sdl_renderer, sdl_texture, &src_rect, NULL);
  }

  // Draw!
  SDL_RenderPresent(sdl_renderer);
#endif
}

//
// I_ScreenShot - moved to i_sshot.c
//

//
// I_SetPalette
//
void I_SetPalette (int pal)
{
  newpal = pal;
}

// I_PreInitGraphics

static void I_ShutdownSDL(void)
{
  SDL_Quit();
  return;
}

void I_PreInitGraphics(void)
{
  int p;

  // Initialize SDL
  unsigned int flags = 0;
  if (!(M_CheckParm("-nodraw") && M_CheckParm("-nosound")))
    flags = SDL_INIT_VIDEO;
#ifdef PRBOOM_DEBUG
  flags |= SDL_INIT_NOPARACHUTE;
#endif

  p = SDL_Init(flags);
  if (p < 0)
  {
    I_Error("Could not initialize SDL [%s]", SDL_GetError());
  }

  atexit(I_ShutdownSDL);
}

// e6y: resolution limitation is removed
void I_InitBuffersRes(void)
{
  R_InitMeltRes();
  R_InitSpritesRes();
  R_InitBuffersRes();
  R_InitPlanesRes();
  R_InitVisplanesRes();
}

#define MAX_RESOLUTIONS_COUNT 128
const char *screen_resolutions_list[MAX_RESOLUTIONS_COUNT] = {NULL};
const char *screen_resolution_lowest;
const char *screen_resolution = NULL;

//
// I_GetScreenResolution
// Get current resolution from the config variable (WIDTHxHEIGHT format)
// 640x480 if screen_resolution variable has wrong data
//
void I_GetScreenResolution(void)
{
  int width, height;

  desired_screenwidth = DEFAULT_SCREEN_W;
  desired_screenheight = DEFAULT_SCREEN_H;

  if (screen_resolution)
  {
    if (sscanf(screen_resolution, "%dx%d", &width, &height) == 2)
    {
      desired_screenwidth = width;
      desired_screenheight = height;
    }
  }
}

//
// I_FillScreenResolutionsList
// Get all the supported screen resolutions
// and fill the list with them
//
static void I_FillScreenResolutionsList(void)
{
  int display_index = 0;
  SDL_DisplayMode mode;
  int i, j, list_size, current_resolution_index, count;
  char mode_name[256];

  // do it only once
  if (screen_resolutions_list[0])
  {
    return;
  }

  if (desired_screenwidth == 0 || desired_screenheight == 0)
  {
    I_GetScreenResolution();
  }

  // Don't call SDL_ListModes if SDL has not been initialized
  count = 0;
  if (!nodrawers)
    count = I_GetNumDisplayModes(display_index);

  list_size = 0;
  current_resolution_index = -1;

  if (count > 0)
  {
    count = MIN(count, MAX_RESOLUTIONS_COUNT - 2);

    for(i = count - 1; i >= 0; i--)
    {
      int in_list = false;

      I_GetDisplayMode(display_index, i, &mode);

      doom_snprintf(mode_name, sizeof(mode_name), "%dx%d", mode.w, mode.h);

      if (i == count - 1)
      {
        screen_resolution_lowest = strdup(mode_name);
      }
      
      for(j = 0; j < list_size; j++)
      {
        if (!strcmp(mode_name, screen_resolutions_list[j]))
        {
          in_list = true;
          break;
        }
      }

      if (!in_list)
      {
        screen_resolutions_list[list_size] = strdup(mode_name);

        if (mode.w == desired_screenwidth && mode.h == desired_screenheight)
        {
          current_resolution_index = list_size;
        }

        list_size++;
      }
    }
    screen_resolutions_list[list_size] = NULL;
  }
  
  if (list_size == 0)
  {
    doom_snprintf(mode_name, sizeof(mode_name), "%dx%d", desired_screenwidth, desired_screenheight);
    screen_resolutions_list[0] = strdup(mode_name);
    current_resolution_index = 0;
    list_size = 1;
  }

  if (current_resolution_index == -1)
  {
    doom_snprintf(mode_name, sizeof(mode_name), "%dx%d", desired_screenwidth, desired_screenheight);

    // make it first
    list_size++;
    for(i = list_size - 1; i > 0; i--)
    {
      screen_resolutions_list[i] = screen_resolutions_list[i - 1];
    }
    screen_resolutions_list[0] = strdup(mode_name);
    current_resolution_index = 0;
  }

  screen_resolutions_list[list_size] = NULL;
  screen_resolution = screen_resolutions_list[current_resolution_index];
}

// e6y
// GLBoom use this function for trying to set the closest supported resolution if the requested mode can't be set correctly.
// For example glboom.exe -geom 1025x768 -nowindow will set 1024x768.
// It should be used only for fullscreen modes.
static void I_ClosestResolution (int *width, int *height)
{
  int display_index = 0;
  int twidth, theight;
  int cwidth = 0, cheight = 0;
  int i, count;
  unsigned int closest = UINT_MAX;
  unsigned int dist;

  if (!SDL_WasInit(SDL_INIT_VIDEO))
    return;

  count = I_GetNumDisplayModes(display_index);

  if (count > 0)
  {
    for(i=0; i<count; ++i)
    {
      SDL_DisplayMode mode;
      I_GetDisplayMode(display_index, i, &mode);

      twidth = mode.w;
      theight = mode.h;

      if (twidth == *width && theight == *height)
        return;

      //if (iteration == 0 && (twidth < *width || theight < *height))
      //  continue;

      dist = (twidth - *width) * (twidth - *width) + 
             (theight - *height) * (theight - *height);

      if (dist < closest)
      {
        closest = dist;
        cwidth = twidth;
        cheight = theight;
      }
    }
    if (closest != 4294967295u)
    {
      *width = cwidth;
      *height = cheight;
      return;
    }
  }
}  

int process_affinity_mask;
int process_priority;

// e6y
// It is a simple test of CPU cache misses.
unsigned int I_TestCPUCacheMisses(int width, int height, unsigned int mintime)
{
  int i, k;
  char *s, *d, *ps, *pd;
  unsigned int tickStart;
  
  s = (char*)malloc(width * height);
  d = (char*)malloc(width * height);

  tickStart = SDL_GetTicks();
  k = 0;
  do
  {
    ps = s;
    pd = d;
    for(i = 0; i < height; i++)
    {
      pd[0] = ps[0];
      pd += width;
      ps += width;
    }
    k++;
  }
  while (SDL_GetTicks() - tickStart < mintime);

  free(d);
  free(s);

  return k;
}

// CPhipps -
// I_CalculateRes
// Calculates the screen resolution, possibly using the supplied guide
void I_CalculateRes(int width, int height)
{
// e6y
// GLBoom will try to set the closest supported resolution 
// if the requested mode can't be set correctly.
// For example glboom.exe -geom 1025x768 -nowindow will set 1024x768.
// It affects only fullscreen modes.
  if (V_GetMode() == VID_MODEGL) {
#ifdef __vita__
    // always run in max res in GL mode
    width = DEFAULT_SCREEN_W;
    height = DEFAULT_SCREEN_H;
#endif
    if ( desired_fullscreen )
    {
      I_ClosestResolution(&width, &height);
    }
    SCREENWIDTH = width;
    SCREENHEIGHT = height;
    SCREENPITCH = SCREENWIDTH;
  } else {
    unsigned int count1, count2;
    int pitch1, pitch2;

    SCREENWIDTH = width;//(width+15) & ~15;
    SCREENHEIGHT = height;

    // e6y
    // Trying to optimise screen pitch for reducing of CPU cache misses.
    // It is extremally important for wiping in software.
    // I have ~20x improvement in speed with using 1056 instead of 1024 on Pentium4
    // and only ~10% for Core2Duo
    if (1)
    {
      unsigned int mintime = 100;
      int w = (width+15) & ~15;
      pitch1 = w * V_GetPixelDepth();
      pitch2 = w * V_GetPixelDepth() + 32;

      count1 = I_TestCPUCacheMisses(pitch1, SCREENHEIGHT, mintime);
      count2 = I_TestCPUCacheMisses(pitch2, SCREENHEIGHT, mintime);

      lprintf(LO_INFO, "I_CalculateRes: trying to optimize screen pitch\n");
      lprintf(LO_INFO, " test case for pitch=%d is processed %d times for %d msec\n", pitch1, count1, mintime);
      lprintf(LO_INFO, " test case for pitch=%d is processed %d times for %d msec\n", pitch2, count2, mintime);

      SCREENPITCH = (count2 > count1 ? pitch2 : pitch1);

      lprintf(LO_INFO, " optimized screen pitch is %d\n", SCREENPITCH);
    }
    else
    {
      SCREENPITCH = SCREENWIDTH * V_GetPixelDepth();
    }
  }

  // e6y: processing of screen_multiply
  {
    if (render_screen_multiply > 0)
    {
      // old fixed scale
      const int factor = ((V_GetMode() == VID_MODEGL) ? 1 : render_screen_multiply);
      REAL_SCREENWIDTH = SCREENWIDTH * factor;
      REAL_SCREENHEIGHT = SCREENHEIGHT * factor;
      REAL_SCREENPITCH = SCREENPITCH * factor;
    }
#ifdef __vita__
    else
    {
      double xfactor, yfactor;
      switch (render_screen_multiply)
      {
        case VID_SCALE_ASPECT:
          xfactor = yfactor = (double)DEFAULT_SCREEN_H / (double)SCREENHEIGHT;
          break;
        case VID_SCALE_INT:
          xfactor = yfactor = DEFAULT_SCREEN_H / SCREENHEIGHT;
          break;
        default:
          xfactor = (double)DEFAULT_SCREEN_W / (double)SCREENWIDTH;
          yfactor = (double)DEFAULT_SCREEN_H / (double)SCREENHEIGHT;
          break;
      }
      // do a ceil here just in case
      REAL_SCREENWIDTH = SCREENWIDTH * xfactor + 0.5;
      REAL_SCREENHEIGHT = SCREENHEIGHT * yfactor + 0.5;
      REAL_SCREENPITCH = SCREENPITCH * xfactor + 0.5;
    }
  }
#endif
}

// CPhipps -
// I_InitScreenResolution
// Sets the screen resolution
// e6y: processing of screen_multiply
void I_InitScreenResolution(void)
{
  int i, p, w, h;
  char c, x;
  video_mode_t mode;
  int init = (screen == NULL);

  I_GetScreenResolution();

  if (init)
  {
    //e6y: ability to change screen resolution from GUI
    I_FillScreenResolutionsList();

    // Video stuff
    if ((p = M_CheckParm("-width")))
      if (myargv[p+1])
        desired_screenwidth = atoi(myargv[p+1]);

    if ((p = M_CheckParm("-height")))
      if (myargv[p+1])
        desired_screenheight = atoi(myargv[p+1]);

#ifdef __vita__
    use_fullscreen = 1;
#endif

    if ((p = M_CheckParm("-fullscreen")))
      use_fullscreen = 1;

    if ((p = M_CheckParm("-nofullscreen")))
      use_fullscreen = 0;

    // e6y
    // New command-line options for setting a window (-window) 
    // or fullscreen (-nowindow) mode temporarily which is not saved in cfg.
    // It works like "-geom" switch
    desired_fullscreen = use_fullscreen;
    if ((p = M_CheckParm("-window")))
      desired_fullscreen = 0;

    if ((p = M_CheckParm("-nowindow")))
      desired_fullscreen = 1;

    // e6y
    // change the screen size for the current session only
    // syntax: -geom WidthxHeight[w|f]
    // examples: -geom 320x200f, -geom 640x480w, -geom 1024x768
    w = desired_screenwidth;
    h = desired_screenheight;

    if (!(p = M_CheckParm("-geom")))
      p = M_CheckParm("-geometry");

    if (p && p + 1 < myargc)
    {
      int count = sscanf(myargv[p+1], "%d%c%d%c", &w, &x, &h, &c);

      // at least width and height must be specified
      // restoring original values if not
      if (count < 3 || tolower(x) != 'x')
      {
        w = desired_screenwidth;
        h = desired_screenheight;
      }
      else
      {
        if (count >= 4)
        {
          if (tolower(c) == 'w')
            desired_fullscreen = 0;
          if (tolower(c) == 'f')
            desired_fullscreen = 1;
        }
      }
    }
  }
  else
  {
    w = desired_screenwidth;
    h = desired_screenheight;
  }

  mode = (video_mode_t)I_GetModeFromString(default_videomode);
  if ((i=M_CheckParm("-vidmode")) && i<myargc-1)
  {
    mode = (video_mode_t)I_GetModeFromString(myargv[i+1]);
  }
  
  V_InitMode(mode);

  I_CalculateRes(w, h);
  V_DestroyUnusedTrueColorPalettes();
  V_FreeScreens();

  // set first three to standard values
  for (i=0; i<3; i++) {
    screens[i].width = REAL_SCREENWIDTH;
    screens[i].height = REAL_SCREENHEIGHT;
    screens[i].byte_pitch = REAL_SCREENPITCH;
    screens[i].short_pitch = REAL_SCREENPITCH / V_GetModePixelDepth(VID_MODE16);
    screens[i].int_pitch = REAL_SCREENPITCH / V_GetModePixelDepth(VID_MODE32);
  }

  // statusbar
  screens[4].width = REAL_SCREENWIDTH;
  screens[4].height = REAL_SCREENHEIGHT;
  screens[4].byte_pitch = REAL_SCREENPITCH;
  screens[4].short_pitch = REAL_SCREENPITCH / V_GetModePixelDepth(VID_MODE16);
  screens[4].int_pitch = REAL_SCREENPITCH / V_GetModePixelDepth(VID_MODE32);

  I_InitBuffersRes();

  lprintf(LO_INFO,"I_InitScreenResolution: Using resolution %dx%d\n", REAL_SCREENWIDTH, REAL_SCREENHEIGHT);
}

// 
// Set the window caption
//

void I_SetWindowCaption(void)
{
  SDL_SetWindowTitle(NULL, PACKAGE_NAME " " PACKAGE_VERSION);
}

// 
// Set the application icon
// 

#include "icon.c"

void I_SetWindowIcon(void)
{
  static SDL_Surface *surface = NULL;

  // do it only once, because of crash in SDL_InitVideoMode in SDL 1.3
  if (!surface)
  {
    surface = SDL_CreateRGBSurfaceFrom(icon_data,
      icon_w, icon_h, 32, icon_w * 4,
      0xff << 0, 0xff << 8, 0xff << 16, 0xff << 24);
  }

  if (surface)
  {
    SDL_SetWindowIcon(NULL, surface);
  }
}

void I_InitGraphics(void)
{
  static int    firsttime=1;

  if (firsttime)
  {
#ifdef __vita__
    // init vitaGL once at the very start
    // use max resolution; in software mode we'll just change the resolution of the
    // framebuffer texture, in hardware mode there's little reason to use small res
    const enum SceGxmMultisampleMode gxm_ms =
      render_multisampling <= 1 ? SCE_GXM_MULTISAMPLE_NONE :
      render_multisampling == 2 ? SCE_GXM_MULTISAMPLE_2X :
                                  SCE_GXM_MULTISAMPLE_4X;
    vglInitExtended(0x800000, DEFAULT_SCREEN_W, DEFAULT_SCREEN_H, 0x1000000, gxm_ms);
    vglWaitVblankStart(GL_TRUE);
    // create a fake SDL window for events and shit
    sdl_window = SDL_CreateWindow(
      PACKAGE_NAME " " PACKAGE_VERSION,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      REAL_SCREENWIDTH, REAL_SCREENHEIGHT,
      0);
    if (!sdl_window)
      I_Error("SDL window dead: %s", SDL_GetError());
#endif

    firsttime = 0;

    atexit(I_ShutdownGraphics);
    lprintf(LO_INFO, "I_InitGraphics: %dx%d\n", SCREENWIDTH, SCREENHEIGHT);

    /* Set the video mode */
    I_UpdateVideoMode();

    //e6y: setup the window title
    I_SetWindowCaption();

    //e6y: set the application icon
    I_SetWindowIcon();

    /* Initialize the input system */
    I_InitInputs();

    //e6y: new mouse code
    UpdateFocus();
    UpdateGrab();
  }
}

int I_GetModeFromString(const char *modestr)
{
  video_mode_t mode;

  if (!stricmp(modestr,"15")) {
    mode = VID_MODE15;
  } else if (!stricmp(modestr,"15bit")) {
    mode = VID_MODE15;
  } else if (!stricmp(modestr,"16")) {
    mode = VID_MODE16;
  } else if (!stricmp(modestr,"16bit")) {
    mode = VID_MODE16;
  } else if (!stricmp(modestr,"32")) {
    mode = VID_MODE32;
  } else if (!stricmp(modestr,"32bit")) {
    mode = VID_MODE32;
  } else if (!stricmp(modestr,"gl")) {
    mode = VID_MODEGL;
  } else if (!stricmp(modestr,"OpenGL")) {
    mode = VID_MODEGL;
  } else {
    mode = VID_MODE8;
  }

  return mode;
}

void I_UpdateVideoMode(void)
{
  int init_flags = 0;
  const dboolean novsync = M_CheckParm("-timedemo") || \
                           M_CheckParm("-fastdemo");

  if(screen)
  {
    // video capturing cannot be continued with new screen settings
    I_CaptureFinish();

#ifdef GL_DOOM
    if (V_GetMode() == VID_MODEGL)
    {
      gld_CleanMemory();
      // hires patches
      gld_CleanStaticMemory();
    }
#endif

    I_InitScreenResolution();

#ifdef __vita__
    if (sw_texture)
    {
      glDeleteTextures(1, &sw_texture);
      sw_texture = 0;
      sw_texptr = NULL;
    }
#else
    SDL_GL_DeleteContext(sdl_glcontext);
    SDL_DestroyTexture(sdl_texture);
    SDL_DestroyTexture(sdl_texture_upscaled);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
#endif

    SDL_FreeSurface(screen);
    if (buffer != screen)
      SDL_FreeSurface(buffer);

    sdl_renderer = NULL;
    sdl_window = NULL;
    screen = NULL;
  }

  // e6y: initialisation of screen_multiply
  screen_multiply = render_screen_multiply;

  // Initialize SDL with this graphics mode
  if (V_GetMode() == VID_MODEGL) {
    init_flags = SDL_WINDOW_OPENGL;
  }

  // Fullscreen desktop for software renderer only - DTIED
  if (desired_fullscreen && V_GetMode() != VID_MODEGL)
    init_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  else
  if ( desired_fullscreen )
    init_flags |= SDL_WINDOW_FULLSCREEN;

  // In windowed mode, the window can be resized while the game is
  // running.  This feature is disabled on OS X, as it adds an ugly
  // scroll handle to the corner of the screen.
#ifndef MACOSX
  if (!desired_fullscreen)
    init_flags |= SDL_WINDOW_RESIZABLE;
#endif

  if (V_GetMode() == VID_MODEGL)
  {
#ifdef GL_DOOM
    //e6y: anti-aliasing
    gld_MultisamplingInit();

#ifdef __vita__
    // only use VRAM in GL mode because transfering textures every frame
    // is fucking slow
    vglUseVram(GL_TRUE);

    // allocate enough vertices for more than one quad
    vglSetImmediateBufferSize(32768);

    glClearColor(0, 0, 0, 0);

    // clear matrices possibly stuck there from previous usage of sw modes
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gld_CheckHardwareGamma();
#else
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 0 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, gl_colorbuffer_bits );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, gl_depthbuffer_bits );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    sdl_window = SDL_CreateWindow(
      PACKAGE_NAME " " PACKAGE_VERSION,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      REAL_SCREENWIDTH, REAL_SCREENHEIGHT,
      init_flags);
    sdl_glcontext = SDL_GL_CreateContext(sdl_window);

    gld_CheckHardwareGamma();
#endif
#endif
  }
  else
  {
    int flags;

    screen = SDL_CreateRGBSurface(0, SCREENWIDTH, SCREENHEIGHT, V_GetNumPixelBits(), 0, 0, 0, 0);
    if(screen == NULL) {
      I_Error("Couldn't set %dx%d video mode [%s]", REAL_SCREENWIDTH, REAL_SCREENHEIGHT, SDL_GetError());
    }

#ifdef __vita__
    if (V_GetMode() != VID_MODE8)
    {
      // no use converting anything
      buffer = screen;
    }
    else
    {
      buffer = SDL_CreateRGBSurface(0, SCREENWIDTH, SCREENHEIGHT, 32, 0, 0, 0, 0);
      SDL_FillRect(buffer, NULL, 0);
    }

    // we only need enough for 1 quad, but allocate a bit more just in case
    vglSetImmediateBufferSize(256);

    // we're gonna be drawing fullscreen quads, so set normal orthographic projection
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, DEFAULT_SCREEN_W, DEFAULT_SCREEN_H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // don't need depth, texture is always used
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // if the video mode is not indexed, we can use the screen surface as is
    // otherwise we'll need to convert
    switch (V_GetMode())
    {
      case VID_MODE16:
        sw_textype = GL_UNSIGNED_SHORT_5_6_5;
        sw_texfmt = GL_RGB;
        sw_texsize = SCREENWIDTH * SCREENHEIGHT * 2;
        break;
      case VID_MODE15:
        sw_textype = GL_UNSIGNED_SHORT_5_5_5_1;
        sw_texfmt = GL_RGBA;
        sw_texsize = SCREENWIDTH * SCREENHEIGHT * 2;
        break;
      default:
        sw_textype = GL_UNSIGNED_BYTE;
        sw_texfmt = GL_BGRA;
        sw_texsize = SCREENWIDTH * SCREENHEIGHT * 4;
        break;
    }

    // generate frame texture and fill it with black (have to do it so vitaGL would allocate it)
    glGenTextures(1, &sw_texture);
    glBindTexture(GL_TEXTURE_2D, sw_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, sw_texfmt, SCREENWIDTH, SCREENHEIGHT, 0, sw_texfmt, sw_textype, screen->pixels);

    // get data pointer so we can just memcpy later
    sw_texptr = vglGetTexDataPointer(GL_TEXTURE_2D);

    // make buffer point to the actual texture data to avoid extra memcpying
    if (buffer->pixels) SDL_free(buffer->pixels);
    buffer->flags |= SDL_PREALLOC;
    buffer->pixels = sw_texptr;
#else
    sdl_window = SDL_CreateWindow(
      PACKAGE_NAME " " PACKAGE_VERSION,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      REAL_SCREENWIDTH, REAL_SCREENHEIGHT,
      init_flags);

    buffer = SDL_CreateRGBSurface(0, REAL_SCREENWIDTH, REAL_SCREENHEIGHT, 32, 0, 0, 0, 0);
    SDL_FillRect(buffer, NULL, 0);

    flags = SDL_RENDERER_TARGETTEXTURE;

    if (render_vsync && !novsync)
      flags |= SDL_RENDERER_PRESENTVSYNC;

    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, flags);

    SDL_RenderSetLogicalSize(sdl_renderer, REAL_SCREENWIDTH, REAL_SCREENHEIGHT);

    sdl_texture = SDL_CreateTextureFromSurface(sdl_renderer, buffer);

    if (screen_multiply)
    {
      sdl_texture_upscaled = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET, REAL_SCREENWIDTH, REAL_SCREENHEIGHT);
    }
#endif
  }

  if (sdl_video_window_pos)
  {
    int x, y;
    if (sscanf(sdl_video_window_pos, "%d,%d", &x, &y) == 2)
    {
      SDL_SetWindowPosition(sdl_window, x, y);
    }
    if (strcmp(sdl_video_window_pos, "center") == 0)
    {
      SDL_SetWindowPosition(sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
  }

  windowid = SDL_GetWindowID(sdl_window);

#ifdef GL_DOOM
  if (V_GetMode() == VID_MODEGL)
  {
    SDL_GL_SetSwapInterval(((render_vsync && !novsync) ? 1 : 0));
  }
#endif

#ifdef GL_DOOM
  /*if (V_GetMode() == VID_MODEGL)
    gld_MultisamplingCheck();*/
#endif

  if (V_GetMode() != VID_MODEGL)
  {
    lprintf(LO_INFO, "I_UpdateVideoMode: 0x%x, %s, %s\n", init_flags, screen && screen->pixels ? "SDL buffer" : "own buffer", screen && SDL_MUSTLOCK(screen) ? "lock-and-copy": "direct access");

    // Get the info needed to render to the display
#ifdef __vita__
    if (1)
#else
    if (screen_multiply==1 && !SDL_MUSTLOCK(screen))
#endif
    {
      screens[0].not_on_heap = true;
      screens[0].data = (unsigned char *) (screen->pixels);
      screens[0].byte_pitch = screen->pitch;
      screens[0].short_pitch = screen->pitch / V_GetModePixelDepth(VID_MODE16);
      screens[0].int_pitch = screen->pitch / V_GetModePixelDepth(VID_MODE32);
    }
    else
    {
      screens[0].not_on_heap = false;
    }

    V_AllocScreens();

    R_InitBuffer(SCREENWIDTH, SCREENHEIGHT);
  }

  // e6y: wide-res
  // Need some initialisations before level precache
  R_ExecuteSetViewSize();

  V_SetPalette(0);
  I_UploadNewPalette(0, true);

  ST_SetResolution();
  AM_SetResolution();

#ifdef GL_DOOM
  if (V_GetMode() == VID_MODEGL)
  {
    int temp;
    lprintf(LO_INFO,"SDL OpenGL PixelFormat:\n");
    SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_RED_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_GREEN_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_BLUE_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_STENCIL_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_ACCUM_RED_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_ACCUM_RED_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_ACCUM_GREEN_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_ACCUM_GREEN_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_ACCUM_BLUE_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_ACCUM_BLUE_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_ACCUM_ALPHA_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &temp );
    lprintf(LO_INFO,"    SDL_GL_DOUBLEBUFFER: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_BUFFER_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_BUFFER_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_DEPTH_SIZE: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &temp );
    lprintf(LO_INFO,"    SDL_GL_MULTISAMPLESAMPLES: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &temp );
    lprintf(LO_INFO,"    SDL_GL_MULTISAMPLEBUFFERS: %i\n",temp);
    SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &temp );
    lprintf(LO_INFO,"    SDL_GL_STENCIL_SIZE: %i\n",temp);

    gld_Init(SCREENWIDTH, SCREENHEIGHT);
  }

  if (V_GetMode() == VID_MODEGL)
  {
    M_ChangeFOV();
    deh_changeCompTranslucency();
  }
#endif

  src_rect.w = SCREENWIDTH;
  src_rect.h = SCREENHEIGHT;
  dst_rect.w = REAL_SCREENWIDTH;
  dst_rect.h = REAL_SCREENHEIGHT;
#ifdef __vita__
  // center the dest quad vertically and horizontally
  dst_rect.x = (DEFAULT_SCREEN_W - REAL_SCREENWIDTH) / 2;
  dst_rect.y = (DEFAULT_SCREEN_H - REAL_SCREENHEIGHT) / 2;
#endif
}

static void ActivateMouse(void)
{
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

static void DeactivateMouse(void)
{
  SDL_SetRelativeMouseMode(SDL_FALSE);
}

//
// Read the change in mouse state to generate mouse motion events
//
// This is to combine all mouse movement for a tic into one mouse
// motion event.
static void I_ReadMouse(void)
{
  if (!usemouse)
    return;

  if (!MouseShouldBeGrabbed())
  {
    mouse_currently_grabbed = false;
    return;
  }

  if (!mouse_currently_grabbed && !desired_fullscreen)
  {
    mouse_currently_grabbed = true;
  }
}

static dboolean MouseShouldBeGrabbed()
{
  // never grab the mouse when in screensaver mode

  //if (screensaver_mode)
  //    return false;

  // if the window doesnt have focus, never grab it
  if (!window_focused)
    return false;

  // always grab the mouse when full screen (dont want to 
  // see the mouse pointer)
  if (desired_fullscreen)
    return true;

  // if we specify not to grab the mouse, never grab
  if (!mouse_enabled)
    return false;

  // always grab the mouse in camera mode when playing levels 
  // and menu is not active
  if (walkcamera.type)
    return (demoplayback && gamestate == GS_LEVEL && !menuactive);

  // when menu is active or game is paused, release the mouse 
  if (menuactive || paused)
    return false;

  // only grab mouse when playing levels (but not demos)
  return (gamestate == GS_LEVEL) && !demoplayback;
}

// Update the value of window_focused when we get a focus event
//
// We try to make ourselves be well-behaved: the grab on the mouse
// is removed if we lose focus (such as a popup window appearing),
// and we dont move the mouse around if we aren't focused either.
static void UpdateFocus(void)
{
  Uint32 flags = 0;

  window_focused = false;
  if(sdl_window)
  {
    flags = SDL_GetWindowFlags(sdl_window);
    if ((flags & SDL_WINDOW_SHOWN) && !(flags & SDL_WINDOW_MINIMIZED) && (flags & SDL_WINDOW_INPUT_FOCUS))
    {
      window_focused = true;
    }
  }

  // e6y
  // Reuse of a current palette to avoid black screen at software fullscreen modes
  // after switching to OS and back
  if (desired_fullscreen && window_focused)
  {
    // currentPaletteIndex?
    if (st_palette < 0)
      st_palette = 0;

    V_SetPalette(st_palette);
  }

#ifdef GL_DOOM
  if (V_GetMode() == VID_MODEGL)
  {
    if (gl_hardware_gamma)
    {
      if (!window_focused)
      {
        // e6y: Restore of startup gamma if window loses focus
        gld_SetGammaRamp(-1);
      }
      else
      {
        gld_SetGammaRamp(useglgamma);
      }
    }
  }
#endif

  // Should the screen be grabbed?
  //    screenvisible = (state & SDL_APPACTIVE) != 0;
}

void UpdateGrab(void)
{
  static dboolean currently_grabbed = false;
  dboolean grab;

  grab = MouseShouldBeGrabbed();

  if (grab && !currently_grabbed)
  {
    ActivateMouse();
  }

  if (!grab && currently_grabbed)
  {
    DeactivateMouse();
  }

  currently_grabbed = grab;
}

static void ApplyWindowResize(SDL_Event *resize_event)
{
  int w = resize_event->window.data1;
  int h = resize_event->window.data2;
  SDL_RenderSetLogicalSize(sdl_renderer, w, w * REAL_SCREENHEIGHT / REAL_SCREENWIDTH);
}
