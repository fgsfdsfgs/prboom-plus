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
 *   Joystick handling for Linux
 *
 *-----------------------------------------------------------------------------
 */

#ifndef lint
#endif /* lint */

#include <stdlib.h>

#include "SDL.h"
#include "doomdef.h"
#include "doomtype.h"
#include "m_argv.h"
#include "d_event.h"
#include "d_main.h"
#include "i_joy.h"
#include "lprintf.h"

#define TRIGGER_DEADZONE 16384

int joyaxis_moveh;
int joyaxis_movev;
int joyaxis_lookh;
int joyaxis_lookv;

// 0-16
int joy_deadzone_left = 1;
int joy_deadzone_right = 1;

int joy_permastrafe = 1;

int usejoystick;

static SDL_GameController *joystick;
static int joystick_num;

static int prev_axis[SDL_CONTROLLER_AXIS_MAX];

static int real_deadzone_left;
static int real_deadzone_right;

static void I_EndJoystick(void)
{
  lprintf(LO_DEBUG, "I_EndJoystick : closing joystick\n");
  if (joystick)
  {
    SDL_GameControllerClose(joystick);
    joystick = NULL;
  }
}

static inline int GetAxis(const int axis)
{
  const int val = SDL_GameControllerGetAxis(joystick, axis);
  const int dz = (axis < 2) ? real_deadzone_left : real_deadzone_right;
  return (abs(val) > dz) ? val : 0;
}

static inline int JoystickMove(const int axis)
{
  int axis_value;
  if (axis >= 0 && axis < SDL_CONTROLLER_AXIS_MAX)
  {
    prev_axis[axis] = GetAxis(axis);
    axis_value = prev_axis[axis] / 3000;
    if (abs(axis_value) < 7) axis_value = 0;
    return axis_value;
  }
  return 0;
}

static inline int JoystickLook(const int axis)
{
  if (axis >= 0 && axis < SDL_CONTROLLER_AXIS_MAX)
  {
    prev_axis[axis] = GetAxis(axis);
    return prev_axis[axis] >> 4;
  }
  return 0;
}

void I_PollJoystick(void)
{
  event_t ev;
  Sint16 axis_value;
  int i;

  if (!usejoystick || !joystick) return;

  real_deadzone_left =  32768.f * (float)joy_deadzone_left  / 16.f;
  real_deadzone_right = 32768.f * (float)joy_deadzone_right / 16.f;

  // movement uses the old joystick system

  ev.type = ev_joystick;
  ev.data1 = 0;
  ev.data2 = JoystickMove(joyaxis_moveh);
  ev.data3 = JoystickMove(joyaxis_movev);
  D_PostEvent(&ev);

  // look translates to mouse motion

  ev.type = ev_mouse;
  ev.data1 = 0;
  ev.data2 = JoystickLook(joyaxis_lookh);
  ev.data3 = -JoystickLook(joyaxis_lookv);
  if (ev.data2 || ev.data3) D_PostEvent(&ev);

  // triggers generate keypresses

  ev.data2 = ev.data3 = 0;
  for (i = SDL_CONTROLLER_AXIS_TRIGGERLEFT; i <= SDL_CONTROLLER_AXIS_TRIGGERRIGHT; ++i)
  {
    axis_value = SDL_GameControllerGetAxis(joystick, i);
    ev.data1 = KEYD_JOY_BASE + i;
    if (axis_value >= TRIGGER_DEADZONE && prev_axis[i] < TRIGGER_DEADZONE)
    {
      ev.type = ev_keydown;
      D_PostEvent(&ev);
    }
    else if (axis_value < TRIGGER_DEADZONE && prev_axis[i] >= TRIGGER_DEADZONE)
    {
      ev.type = ev_keyup;
      D_PostEvent(&ev);
    }
  }
}

void I_InitJoystick(void)
{
  const char* fname = "I_InitJoystick : ";
  int num_joysticks;

  if (!usejoystick || M_CheckParm("-nojoy")) return;

  SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
  num_joysticks = SDL_NumJoysticks();

  if ((usejoystick > num_joysticks) || (usejoystick <= 0) || !SDL_IsGameController(usejoystick-1))
  {
    lprintf(LO_WARN, "%sinvalid joystick %d\n", fname, usejoystick);
    return;
  }

  joystick = SDL_GameControllerOpen(usejoystick-1);
  if (!joystick)
  {
    lprintf(LO_ERROR, "%serror opening joystick %d\n", fname, usejoystick);
  }
  else
  {
    atexit(I_EndJoystick);
    SDL_GameControllerEventState(SDL_ENABLE);
    lprintf(LO_INFO, "%sopened %s\n", fname, SDL_GameControllerName(joystick));
  }
}
