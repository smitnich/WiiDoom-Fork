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
 *  Misc system stuff needed by Doom, implemented for Linux.
 *  Mainly timer handling, and ENDOOM/ENDBOOM.
 *
 *-----------------------------------------------------------------------------
 */

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#ifdef _MSC_VER
#define    F_OK    0    /* Check for file existence */
#define    W_OK    2    /* Check for write permission */
#define    R_OK    4    /* Check for read permission */
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

#include "SDL/SDL.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#ifndef PRBOOM_SERVER
#include "m_argv.h"
#endif
#include "lprintf.h"
#include "doomtype.h"
#include "doomdef.h"
#include "lprintf.h"
#ifndef PRBOOM_SERVER
#include "m_fixed.h"
#include "r_fps.h"
#endif
#include "i_system.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static unsigned int start_displaytime;
static unsigned int displaytime;
static bool InDisplay = false;

bool I_StartDisplay(void)
{
  if (InDisplay)
    return false;

  start_displaytime = SDL_GetTicks();
  InDisplay = true;
  return true;
}

void I_EndDisplay(void)
{
  displaytime = SDL_GetTicks() - start_displaytime;
  InDisplay = false;
}

void I_uSleep(unsigned long usecs)
{
    SDL_Delay(usecs/1000);
}

int ms_to_next_tick;

int I_GetTime_RealTime (void)
{
  int t = SDL_GetTicks();
  int i = t*(TICRATE/5)/200;
  ms_to_next_tick = (i+1)*200/(TICRATE/5) - t;
  if (ms_to_next_tick > 1000/TICRATE || ms_to_next_tick<1) ms_to_next_tick = 1;
  return i;
}

#ifndef PRBOOM_SERVER
fixed_t I_GetTimeFrac (void)
{
  unsigned long now;
  fixed_t frac;

  now = SDL_GetTicks();

  if (tic_vars.step == 0)
    return FRACUNIT;
  else
  {
    frac = (fixed_t)((now - tic_vars.start + displaytime) * FRACUNIT / tic_vars.step);
    if (frac < 0)
      frac = 0;
    if (frac > FRACUNIT)
      frac = FRACUNIT;
    return frac;
  }
}

void I_GetTime_SaveMS(void)
{
  if (!movement_smooth)
    return;

  tic_vars.start = SDL_GetTicks();
  tic_vars.next = (unsigned int) ((tic_vars.start * tic_vars.msec + 1.0f) / tic_vars.msec);
  tic_vars.step = tic_vars.next - tic_vars.start;
}
#endif

/*
 * I_GetRandomTimeSeed
 *
 * CPhipps - extracted from G_ReloadDefaults because it is O/S based
 */
unsigned long I_GetRandomTimeSeed(void)
{
/* This isnt very random */
  return(SDL_GetTicks());
}

/* cphipps - I_GetVersionString
 * Returns a version string in the given buffer
 */
const char* I_GetVersionString(char* buf, size_t sz)
{
#ifdef HAVE_SNPRINTF
  snprintf(buf,sz,"%s v%s (http://prboom.sourceforge.net/)",PACKAGE,VERSION);
#else
  sprintf(buf,"%s v%s (http://prboom.sourceforge.net/)",PACKAGE,VERSION);
#endif
  return buf;
}

/* cphipps - I_SigString
 * Returns a string describing a signal number
 */
const char* I_SigString(char* buf, size_t sz, int signum)
{
#ifdef SYS_SIGLIST_DECLARED
  if (strlen(sys_siglist[signum]) < sz)
    strcpy(buf,sys_siglist[signum]);
  else
#endif
#ifdef HAVE_SNPRINTF
    snprintf(buf,sz,"signal %d",signum);
#else
    sprintf(buf,"signal %d",signum);
#endif
  return buf;
}


/* 
 * I_Read
 *
 * cph 2001/11/18 - wrapper for read(2) which handles partial reads and aborts
 * on error.
 */
void I_Read(int fd, void* vbuf, size_t sz)
{
  unsigned char* buf = vbuf;

  while (sz) {
    int rc = read(fd,buf,sz);
    if (rc <= 0) {
      I_Error("I_Read: read failed: %s", rc ? strerror(errno) : "EOF");
    }
    sz -= rc; buf += rc;
  }
}

/*
 * I_Filelength
 *
 * Return length of an open file.
 */

int I_Filelength(int handle)
{
  struct stat   fileinfo;
  if (fstat(handle,&fileinfo) == -1)
    I_Error("I_Filelength: %s",strerror(errno));
  return fileinfo.st_size;
}

#ifndef PRBOOM_SERVER

// cph - V.Aguilar (5/30/99) suggested return ~/.lxdoom/, creating
//  if non-existant
//static const char prboom_dir[] = {"sd:/prboom"}; // Mead rem extra slash 8/21/03

const char *I_DoomExeDir(void)
{
  static char *base;
  if (!base)        // cache multiple requests
    {
  //Determine SD or USB
  FILE * fp2;
  bool sd = false;
  bool usb = false;
  fp2 = fopen("sd:/apps/wiidoom/data/prboom.wad", "rb");
  if(fp2)
  sd = true;
  if(!fp2){
  fp2 = fopen("usb:/apps/wiidoom/data/prboom.wad", "rb");
  }
  if(fp2 && !sd)
  usb = true;
  
      if(sd)
      base = malloc(strlen("sd:/apps/wiidoom/data") + 1);
	  if(usb)
	  base = malloc(strlen("usb:/apps/wiidoom/data") + 1);
	  
      if(sd)
      strcat(base, "sd:/apps/wiidoom/data");
	  if(usb)
	  strcat(base, "usb:/apps/wiidoom/data");

      //mkdir(base, S_IRUSR | S_IWUSR | S_IXUSR); // Make sure it exists
    }
  return base;
}

/*
 * HasTrailingSlash
 *
 * cphipps - simple test for trailing slash on dir names
 */

bool HasTrailingSlash(const char* dn)
{
  return ( (dn[strlen(dn)-1] == '/')
#if defined(AMIGA)
        || (dn[strlen(dn)-1] == ':')
#endif
          );
}

/*
 * I_FindFile
 *
 * proff_fs 2002-07-04 - moved to i_system
 *
 * cphipps 19/1999 - writen to unify the logic in FindIWADFile and the WAD
 *      autoloading code.
 * Searches the standard dirs for a named WAD file
 * The dirs are listed at the start of the function
 */

#ifndef MACOSX /* OSX defines its search paths elsewhere. */
char* I_FindFile(const char* wfname, const char* ext)
{
  //size_t  pl = strlen(wfname) + strlen(ext) + 4;
  
    	//Determine SD or USB
    FILE * fp2;
    bool sd = false;
	bool usb = false;
    fp2 = fopen("sd:/apps/wiidoom/data/prboom.wad", "rb");
    if(fp2)
    sd = true;
    if(!fp2){
    fp2 = fopen("usb:/apps/wiidoom/data/prboom.wad", "rb");
    }
    if(fp2 && !sd)
    usb = true;
	
	if(fp2);
	fclose(fp2);

	char *p;
	if(sd)
	p = "sd:/apps/wiidoom/data/";
	if(usb)
	p = "usb:/apps/wiidoom/data/";
	
	char *f;
	f = malloc(strlen(p) + strlen(wfname) + 4);
	sprintf(f, "%s%s", p, wfname);
	if (fopen(f, "r"))
	{
		return f;
	}
	else
	{
		return NULL;
	}
}
#endif

#endif // PRBOOM_SERVER
