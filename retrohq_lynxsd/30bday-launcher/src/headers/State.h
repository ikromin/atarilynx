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

#ifndef __LYNX_STATE__
#define __LYNX_STATE__

#include <time.h>
#include "Types.h"

#pragma bss-name (push, "ZEROPAGE")
  extern s8 selectedGame;
  #pragma zpsym("selectedGame")

  extern u8 continueColour;
  #pragma zpsym("continueColour")
#pragma bss-name (pop)


#pragma bss-name (push, "ENGZP")
  extern u8 _engProcessInputs;
  #pragma zpsym("_engProcessInputs")

  extern time_t _engTimePrevious;
  #pragma zpsym("_engTimePrevious")

  extern time_t _engTimeCurrent;
  #pragma zpsym("_engTimeCurrent")

	extern u8 _engTimeElapsed;
  #pragma zpsym("_engTimeElapsed")

  extern u8 _engTimeLag;
  #pragma zpsym("_engTimeLag")

  extern void (*_engFuncProcessInput)(void);
  #pragma zpsym("_engFuncProcessInput")

  extern void (*_engFuncUpdate)(void);
  #pragma zpsym("_engFuncUpdate")

  extern void (*_engFuncRender)(void);
  #pragma zpsym("_engFuncRender")
#pragma bss-name (pop)

typedef enum {
  GS_LOGO = 0,
  GS_SELECT,
  GS_RUN,
  GS_ERROR
} GS_STATE;

extern char img_game00[], img_game01[], img_game02[], img_game03[], img_game04[], img_game05[],
            img_game06[], img_game07[], img_game08[], img_game09[], img_game10[];

extern SCB_REHV_PAL logoGame;
extern u8* gameImages[];
extern char* gameNames[];
extern char* romNames[];
extern const u8 contColourArr[];

void __fastcall__ changeState(GS_STATE newState);

void processInput_Logo();
void update_Logo();
void render_Logo();

void processInput_Select();
void update_Select();
void render_Select();

void processInput_Run();
void update_Run();
void render_Run();

void processInput_Error();
void update_Error();
void render_Error();

#endif // __LYNX_STATE__