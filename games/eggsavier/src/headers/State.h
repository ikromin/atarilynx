/******************************************************************************
 Eggsavier's Cackleberry Rescue

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

#include "Types.h"

extern void (*_processInput)(void);
extern void (*_render)(void);
extern void (*_update)(void);

typedef enum {
  GS_LOGO = 0,
  GS_GAME,
  GS_HELP
} GS_STATE;

extern GS_STATE gameState;
extern u8 processInputs;
extern unsigned int score;
extern u8 gameMode;

void __fastcall__ changeState(GS_STATE newState);

void stateChange_Logo();
void stateChange_Game();
void stateChange_Help();
void processInput_Logo();
void processInput_Game();
void processInput_Help();
void update_Logo();
void update_Game();
void update_Help();
void render_Logo();
void render_Game();
void render_Help();

#endif // __LYNX_STATE__