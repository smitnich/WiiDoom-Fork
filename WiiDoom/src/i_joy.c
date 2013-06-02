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

#include <SDL/SDL.h>
#include "doomdef.h"
#include "doomtype.h"
#include "m_argv.h"
#include "d_event.h"
#include "d_main.h"
#include "i_joy.h"
#include "lprintf.h"

#include <wiiuse/wpad.h>
#include <math.h>

#define PI 3.14159265

int joyleft;
int joyright;
int joyup;
int joydown;

int usejoystick;

int controlNum = 0;
int chosenController = -1;
u16 GCButtonsDown;
int pad_stickx;
int pad_sticky;
int pad_substickx;
int pad_substicky;
#ifdef HAVE_SDL_JOYSTICKGETAXIS
static SDL_Joystick *joystick;
#endif

static void I_EndJoystick(void)
{
  lprintf(LO_DEBUG, "I_EndJoystick : closing joystick\n");
}

void I_PollJoystick(void)
{
  PAD_ScanPads();
  WPAD_ScanPads();
  WPADData *data = WPAD_Data(0);
  ir_t ir;
  WPAD_IR(0, &ir);
  int nun_x, nun_y, center, min, max, btn_a, btn_b, btn_c, btn_z, btn_1, btn_2, btn_l, btn_r, btn_d, btn_u, btn_p, btn_m, btn_h;
  int btn_lastwep, firstwep;
  int i;
  event_t ev;
  Sint16 axis_x, axis_y;
  
  btn_a = 0;
  btn_b = 0;
  btn_c = 0;
  btn_z = 0;
  btn_1 = 0;
  btn_2 = 0;
  btn_l = 0;
  btn_d = 0;
  btn_r = 0;
  btn_u = 0;
  btn_p = 0;
  btn_m = 0;
  btn_h = 0;

  if (chosenController == -1)
  {
	if (data->btns_h & WPAD_BUTTON_HOME)
		chosenController = 0;
	else if (data->btns_h & WPAD_CLASSIC_BUTTON_HOME && data->exp.type == WPAD_EXP_CLASSIC)
		chosenController = 0;
	else for (i = 0; i < 4; i++)
	{
		if (PAD_ButtonsHeld(i) & PAD_BUTTON_START)
		{
			chosenController = 1;
			controlNum = i;
		}
	}
	return;
  }
   //Wiimote + Nunchuk controls
  if(chosenController == 0 && data->exp.type == WPAD_EXP_NUNCHUK){
  nun_x = data->exp.nunchuk.js.pos.x;
  nun_y = data->exp.nunchuk.js.pos.y;

  center = data->exp.nunchuk.js.center.x;
  min = data->exp.nunchuk.js.min.x;
  max = data->exp.nunchuk.js.max.x;

  if (nun_x < center - ((center - min) * 0.1f)) //Left
    axis_x = (1.0f * center - nun_x) / (center - min) * -50.0f;
  else if (nun_x > center + ((max - center) * 0.1f)) //Right
    axis_x = (1.0f * nun_x - center) / (max - center) * 50.0f;
  else //No stick X movement
    axis_x = 0;

  center = data->exp.nunchuk.js.center.y;
  min = data->exp.nunchuk.js.min.y;
  max = data->exp.nunchuk.js.max.y;

  if (nun_y < center - ((center - min) * 0.1f))//Up
    axis_y = (1.0f * center - nun_y) / (center - min) * -50.0f;
  else if (nun_y > center + ((max - center) * 0.1f))//Down
    axis_y = (1.0f * nun_y - center) / (max - center) * 50.0f;
  else//No stick Y movement
    axis_y = 0;

  // doom_printf("\n\n\n  axis_x: %d\n  axis_y: %d", axis_x, axis_y);

  // For some strange reason, the home button is detected as a keypress and mapped to the esc key.
  // I suspect it's SDL-Port at work here but since it's not a dealbreaker, I'm not terribly
  // interested in tracking it down. It does pretty much what I would have it do anyway.

  if (data->btns_h & WPAD_BUTTON_A)
    btn_a = 1;
  if (data->btns_h & WPAD_BUTTON_B)
    btn_b = 1;
  if (data->btns_h & WPAD_NUNCHUK_BUTTON_C)
    btn_c = 1;
  if (data->btns_h & WPAD_NUNCHUK_BUTTON_Z)
    btn_z = 1;
  if (data->btns_h & WPAD_BUTTON_1)
    btn_1 = 1;
  if (data->btns_h & WPAD_BUTTON_2)
    btn_2 = 1;
  if (data->btns_h & WPAD_BUTTON_LEFT)
    btn_l = 1;
  if (data->btns_h & WPAD_BUTTON_DOWN)
    btn_d = 1;
  if (data->btns_h & WPAD_BUTTON_RIGHT)
    btn_r = 1;
  if (data->btns_h & WPAD_BUTTON_UP)
    btn_u = 1;
  if (data->btns_h & WPAD_BUTTON_PLUS)
    btn_p = 1;
  if (data->btns_h & WPAD_BUTTON_MINUS)
    btn_m = 1;
  if (data->btns_h & WPAD_BUTTON_HOME)
    btn_h = 1;

  ev.type = ev_joystick;
  ev.data2 = axis_x; 
  ev.data3 = axis_y;

  //For turning
  if (ir.raw_valid)
    {
      axis_x = ir.ax - 500;  // x virtual res appears to be around 1000
      axis_y = ir.ay - 375;  // y virtual res appears to be around 750
    }
  else
    {
      axis_x = 0;
      axis_y = 0;
    }

  ev.data4 = axis_x;
  ev.data5 = axis_y;
  }
  //End Wiimote + Nunchuk controls
  
  //Classic Controller
  else if (chosenController == 0 && data->exp.type == WPAD_EXP_CLASSIC){
  
  nun_x = data->exp.classic.ljs.pos.x;
  nun_y = data->exp.classic.ljs.pos.y;

  center = data->exp.classic.ljs.center.x;
  min = data->exp.classic.ljs.min.x;
  max = data->exp.classic.ljs.max.x;

  if (nun_x < center - ((center - min) * 0.1f)) //Left
    axis_x = (1.0f * center - nun_x) / (center - min) * -100.0f;
  else if (nun_x > center + ((max - center) * 0.1f)) //Right
    axis_x = (1.0f * nun_x - center) / (max - center) * 100.0f;
  else //No stick X movement
    axis_x = 0;

  center = data->exp.classic.ljs.center.y;
  min = data->exp.classic.ljs.min.y;
  max = data->exp.classic.ljs.max.y;

  if (nun_y < center - ((center - min) * 0.1f))//Up
    axis_y = (1.0f * center - nun_y) / (center - min) * -100.0f;
  else if (nun_y > center + ((max - center) * 0.1f))//Down
    axis_y = (1.0f * nun_y - center) / (max - center) * 100.0f;
  else//No stick Y movement
    axis_y = 0;

  // doom_printf("\n\n\n  axis_x: %d\n  axis_y: %d", axis_x, axis_y);

  // For some strange reason, the home button is detected as a keypress and mapped to the esc key.
  // I suspect it's SDL-Port at work here but since it's not a dealbreaker, I'm not terribly
  // interested in tracking it down. It does pretty much what I would have it do anyway.

  //Use/Open/Select
  if (data->btns_h & WPAD_CLASSIC_BUTTON_A)
    btn_a = 1;
  //Fire
  if (data->btns_h & WPAD_CLASSIC_BUTTON_FULL_R)
    btn_b = 1;
  //Map
  if (data->btns_h & WPAD_CLASSIC_BUTTON_PLUS)
    btn_c = 1;
  //Run
  if (data->btns_h & WPAD_CLASSIC_BUTTON_FULL_L)
  {
    btn_z = 1;
  }
  //Automap follow
  if (data->btns_h & WPAD_CLASSIC_BUTTON_MINUS)
    btn_1 = 1;
  //No idea ....
  if (data->btns_h & WPAD_BUTTON_2)
    btn_2 = 1;
  //Left Weapon Cycle / Pan Map
  if (data->btns_h & WPAD_CLASSIC_BUTTON_LEFT)
    btn_l = 1;
  //Pan map
  if (data->btns_h & WPAD_CLASSIC_BUTTON_DOWN)
    btn_d = 1;
  //Right Weapon Cycle / Pan Map
  if (data->btns_h & WPAD_CLASSIC_BUTTON_RIGHT)
    btn_r = 1;
  //Pan Map
  if (data->btns_h & WPAD_CLASSIC_BUTTON_UP)
    btn_u = 1;
  //Map zoom in
  if (data->btns_h & WPAD_CLASSIC_BUTTON_ZR)
    btn_p = 1;
  //Map Zoom Out
  if (data->btns_h & WPAD_CLASSIC_BUTTON_ZL)
    btn_m = 1;
  //Escape
  if (data->btns_h & WPAD_CLASSIC_BUTTON_HOME)
    btn_h = 1;
  ev.type = ev_joystick;
  ev.data2 = axis_x; 
  ev.data3 = axis_y;

  //For turning
  nun_x = data->exp.classic.rjs.pos.x;
  nun_y = data->exp.classic.rjs.pos.y;

  center = data->exp.classic.rjs.center.x;
  min = data->exp.classic.rjs.min.x;
  max = data->exp.classic.rjs.max.x;

  if (nun_x < center - ((center - min) * 0.1f)) //Left
    axis_x = (1.0f * center - nun_x) / (float) (center - min) * -330.0f;
  else if (nun_x > center + ((max - center) * 0.1f)) //Right
    axis_x = (1.0f * nun_x - center) / (float) (max - center) * 330.0f;
  else //No stick X movement
    axis_x = 0;

  center = data->exp.classic.rjs.center.y;
  min = data->exp.classic.rjs.min.y;
  max = data->exp.classic.rjs.max.y;

  if (nun_y < center - ((center - min) * 0.1f))//Up
    axis_y = (1.0f * center - nun_y) / (center - min) * -130.0f;
  else if (nun_y > center + ((max - center) * 0.1f))//Down
    axis_y = (1.0f * nun_y - center) / (max - center) * 130.0f;
  else//No stick Y movement
    axis_y = 0;
	
  ev.data4 = axis_x;
  ev.data5 = axis_y;
  
  }
  //End Classic Controller
  
  //Gamecube controller
  else if(chosenController == 1){
  GCButtonsDown = PAD_ButtonsHeld(controlNum);
  pad_stickx = PAD_StickX(controlNum);
  pad_sticky = PAD_StickY(controlNum);
  pad_substickx = PAD_SubStickX(controlNum);
  pad_substicky = PAD_SubStickY(controlNum);
  if (abs(pad_stickx) > 10) //Left
    axis_x = pad_stickx*2;
  else //No stick X movement
    axis_x = 0;

  if (abs(pad_sticky) > 10)//Up
    axis_y = pad_sticky*2;
  else
    axis_y = 0;
  if (pad_sticky > 128*0.75)
	btn_z = 1;
  // doom_printf("\n\n\n  axis_x: %d\n  axis_y: %d", axis_x, axis_y);

  // For some strange reason, the home button is detected as a keypress and mapped to the esc key.
  // I suspect it's SDL-Port at work here but since it's not a dealbreaker, I'm not terribly
  // interested in tracking it down. It does pretty much what I would have it do anyway.
  //Automap follow
  if (GCButtonsDown & PAD_BUTTON_B)
    btn_1 = 1;
  //Use/Open/Select
  if (GCButtonsDown & PAD_BUTTON_A)
  {
    btn_a = 1;
  }
  //Map
  if (GCButtonsDown & PAD_TRIGGER_Z)
  {
    btn_c = 1;
  }
  //Run
  /*if (PAD_ButtonsHeld(0) & PAD_TRIGGER_R)
    btn_z = 1;*/
  //Automap follow
  if (GCButtonsDown & PAD_TRIGGER_R)
    btn_b = 1;
  //Left Weapon Cycle / Pan Map
  if (GCButtonsDown & PAD_BUTTON_LEFT)
    btn_l = 1;
  //Pan map
  if (GCButtonsDown & PAD_BUTTON_DOWN)
    btn_d = 1;
  //Right Weapon Cycle / Pan Map
  if (GCButtonsDown & PAD_BUTTON_RIGHT)
    btn_r = 1;
  //Pan Map
  if (GCButtonsDown & PAD_BUTTON_UP)
    btn_u = 1;
  //Map zoom in
  if (GCButtonsDown & PAD_BUTTON_X)
    btn_p = 1;
  //Map Zoom Out
  if (GCButtonsDown & PAD_BUTTON_Y)
    btn_m = 1;
  //Escape
  if (GCButtonsDown & PAD_BUTTON_START)
    btn_h = 1;

  ev.type = ev_joystick;
  ev.data2 = axis_x; 
  ev.data3 = axis_y;

   if (abs(pad_substickx) > 20) //Left
    axis_x = pad_substickx*2;
	else
    axis_x = 0;

  if (abs(pad_substicky) > 20)//Up
    axis_y = pad_substicky*2;
  else//No stick Y movement
    axis_y = 0;
    
  ev.data4 = axis_x;
  ev.data5 = axis_y;
  
  }
  ev.data1 =
    ((btn_b)<<0) |
    ((btn_c)<<1) |
    ((btn_z)<<2) |
    ((btn_a)<<3) |
    ((btn_1)<<4) |
    ((btn_2)<<5) |
    ((btn_l)<<6) |
    ((btn_d)<<7) |
    ((btn_r)<<8) |
    ((btn_u)<<9) |
    ((btn_p)<<10) |
    ((btn_m)<<11) |
    ((btn_h)<<12);
  //End gamecube controller

  D_PostEvent(&ev);
}

void I_InitJoystick(void)
{
#ifdef HAVE_SDL_JOYSTICKGETAXIS
  const char* fname = "I_InitJoystick : ";
  int num_joysticks;

  if (!usejoystick) return;
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  num_joysticks=SDL_NumJoysticks();
  if (M_CheckParm("-nojoy") || (usejoystick>num_joysticks) || (usejoystick<0)) {
    if ((usejoystick > num_joysticks) || (usejoystick < 0))
      lprintf(LO_WARN, "%sinvalid joystick %d\n", fname, usejoystick);
    else
      lprintf(LO_INFO, "%suser disabled\n", fname);
    return;
  }
  joystick=SDL_JoystickOpen(usejoystick-1);
  if (!joystick)
    lprintf(LO_ERROR, "%serror opening joystick %s\n", fname, SDL_JoystickName(usejoystick-1));
  else {
    atexit(I_EndJoystick);
    lprintf(LO_INFO, "%sopened %s\n", fname, SDL_JoystickName(usejoystick-1));
    joyup = 32767;
    joydown = -32768;
    joyright = 32767;
    joyleft = -32768;
  }
#endif
}
