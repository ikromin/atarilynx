/******************************************************************************
 Atari Gamer's Atari Lynx 30th Birthday Competition ROM Launcher

 Copyright (C) 2019 Igor Kromin

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#include "State.h"

/*
 ******************************************************************************
 Global game state variables and function pointer code for state switching.
 ******************************************************************************
 */

SCB_REHV_PAL logoGame = {
	BPP_3 | TYPE_BACKGROUND, REHV | PACKED, 0x0,
  0x0, 0x0,
  25, 5, // offset
	0x0100, 0x0100, // scale
	{0x89,0xAB,0xCD,0xEF,0x89,0xAB,0xCD,0xEF}
};

u8* gameImages[] = { 
  &img_game00[0], &img_game01[0], &img_game02[0], &img_game03[0], &img_game04[0], &img_game05[0],
  &img_game06[0], &img_game07[0], &img_game08[0], &img_game09[0], &img_game10[0]
};

char* gameNames[] = {
  "4TTUDE",               // 01*-
  "Assembloids",          // 02*-
  "Find a way to my <3",  // 03*-
  "Growing Ties",         // 04*-
  "Lynx Quest",           // 05*-
  "Nutmeg",               // 06*-
  "On Duty",              // 07*-
  "Sky Raider",           // 08*-
  "The Inside World",     // 09*-
  "Undergrounders",       // 10*-
  "YNXA"                  // 11*-
};

char* romNames[] = {
  "30bpc19/01.lnx", "30bpc19/02.lnx", "30bpc19/03.lnx", "30bpc19/04.lnx", "30bpc19/05.lnx", "30bpc19/06.lnx",
  "30bpc19/07.lnx", "30bpc19/08.lnx", "30bpc19/09.lnx", "30bpc19/10.lnx", "30bpc19/11.lnx"
};

const u8 contColourArr[] = {1, 2, 6, 2, 7, 2};

#pragma bss-name (push, "ZEROPAGE")
  u8 processInputs;
  #pragma zpsym("processInputs")

  s8 selectedGame;
  #pragma zpsym("selectedGame")

  u8 continueColour;
  #pragma zpsym("continueColour")

  void (*_processInput)(void);
  #pragma zpsym("_processInput")

  void (*_render)(void);
  #pragma zpsym("_render")

  void (*_update)(void);
  #pragma zpsym("_update")
#pragma bss-name (pop)


void __fastcall__ changeState(GS_STATE newState) {
  switch (newState) {
    case GS_LOGO:
      processInputs = 1;
      continueColour = 0;
      _processInput = &processInput_Logo;
      _render = &render_Logo;
      _update = &update_Logo;
      break;

    case GS_SELECT:
      selectedGame = 0;
      _processInput = &processInput_Select;
      _render = &render_Select;
      _update = &update_Select;
      break;
    
    case GS_RUN:
      _processInput = &processInput_Run;
      _render = &render_Run;
      _update = &update_Run;
      break;
    
    case GS_ERROR:
      _processInput = &processInput_Error;
      _render = &render_Error;
      _update = &update_Error;
      break;
  }
}