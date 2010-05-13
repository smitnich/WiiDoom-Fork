WiiDoom 0.4.2
by jendave (David Hudson)

Source Code: http://code.google.com/p/wiidoom

-- Introduction --
WiiDoom is a DOOM port based on PrBoom. It is compatible with all versions of DOOM, DOOM II, and Final DOOM. 
There is an issue with using FreeDoom, so for now, only the official Doom releases are supported.


-- IMPORTANT INFO --
This is an BETA. That means there may be bugs in it. Please report them via this site:
http://wiidoom.googlecode.com


-- Requirements --
* A Wii
* Wii remote and Nunchuk
* This package
* Some method of booting Wii homebrew (i.e. Homebrew Channel)
* Doom 1/2/Final WAD file (shareware or commercial)


-- Instructions --
Homebrew Browser is the preferred method of installing WiiDoom.

Homebrew Channel: Copy the WiiDoom directory (inside the apps directory) into the "apps" folder of your SD card. 
Copy the prboom folder to the root directory of your SD Card.

Copy any Doom WAD files (doom.wad, doom2.wad, etc..) into the "prboom" directory. 
If you don't have a commercial version of Doom, go to the Source Code site to download the shareware version. 
Leave the prboom.wad file in the directory.
Copy any pwads (add-on levels) into the pwads directory, which is inside the prboom directory.

The Wad Loader runs first, allowing you to select the version of doom (IWAD) and any (optional) add-on levels (PWads). 
Select at least the IWAD and the start button will appear. Select Start and Doom loads up.

Once Doom comes up, you'll need to disconnect and reconnect the Nunchuk. 
This is a temporary bug, but seems to be common with Wii homebrew.


-- PWAD usage --
Multiple pwads are supported, using the same logic as the original Doom engine. 
The order you select them in the Wad Loader will be the exact order they're sent to the engine. 
Check the add-on level's instructions to determine if any special order is needed. 
WiiDoom only looks at the .wad files in the pwads directory, so you can store the .txt files that accompany the PWADs in the same directory.


-- WAD Loader Controls --
Exit to Wii		- B
Select			- A
Scroll PWAD list 	- Left/Right on D-pad


-- Game Controls --
Move around   	- Nunchuk Stick
Menu Nav      	- Nunchuk Stick or D-pad
Switch Weapon 	- Left or Right D-pad
Turn left     	- Point Wiimote left
Turn right    	- Point Wiimote right
Fire          	- B
Use/Open      	- A
Run           	- Z
Menu Select   	- A
Automap		  	- C
Automap Pan	  	- Dpad
Automap Zoom  	- + and -
Automap Follow	- 1 


-- Contributors --
Richard L. Bartlett
David Hudson
Funkamatic
jonathan.buchanan

-- Version 0.4.2 --
  [FIXED]  Recompiled with devkitPro r21 and latest version of SDL-wii.  
  
-- Version 0.4.1 --
  [FIXED]   Fixed crash when loading the shareware wad

-- Version 0.4 --
  [NEW]     Added WAD loader
  [FIXED]   Finale cast can now be advanced through
  [FIXED]   Config file saving/loading works
  [CHANGED] Classis stairbuilding enabled by default
  [NEW]     Added IR crosshair option in general settings menu
  [CHANGED] Disabled IR crosshair by default
  [FIXED]   Fixed vertical resolution to fit on the screen better
  [FIXED]   Fixed Twilight Hack Input bug
  [CHANGED] Moved messages output location to original spot
  [FIXED]   "Reset to defaults" now works

-- Version 0.3.1 --
  [CHANGED] IR input based on raw values now
  [FIXED]   Switching from BFG no longer causes lockup
  [NEW]     Aiming crosshair (eyecandy only)

-- Version 0.3 --
  [NEW]     Saving/Loading working via Nintendo-style save slots
  [FIXED]   Messages display on screen
  [NEW]     Boot.dol is now distributed instead of boot.elf (smaller size)
  [NEW]     Automap implemented
  [NEW]     Preliminary PWAD support

-- Version 0.2 --
  [NEW]     Remapped Wii controls to joystick handling code
  [FIXED]   Control stick no longer stalls
  [NEW]     Joystick code extended to handle nearly all currently supported device inputs
  [NEW]     IR controls partly implemented
  [FIXED]   Menu navigation made consistent with Wii titles
  [FIXED]   Audio now plays at correct pitch
  [FIXED]   Brightness/Gamma set to low value
  [NEW]     Framerate uncapped
  [NEW]     Weapon switching via Left/Right on D-pad
  [CHANGED] IR is now linear

-- Version 0.1.1 --
  [FIXED]   "Quit" bug, and other places where "Y" was required to continue
  [CHANGED] WiiDoom icon to Funkamatic's
