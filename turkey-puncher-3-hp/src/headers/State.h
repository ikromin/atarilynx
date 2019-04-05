/******************************************************************************
 Turkey Puncher 3 Turbo: Hyper Punching. An April Fools' parody game.

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
#include "StateLogo.h"
#include "StatePsyched.h"
#include "StatePlaying.h"
#include "StateEndGame.h"

extern void (*_processInput)(void);
extern void (*_render)(void);
extern void (*_update)(void);

typedef enum {
  GS_LOGO = 0,
  GS_PSYCHED,
  GS_PLAYING,
  GS_ENDGAME
} GS_STATE;

extern GS_STATE gameState;
extern u8 processInputs;

extern u16 score;
extern u8 lastScore;

void __fastcall__ changeState(GS_STATE newState);

#endif // __LYNX_STATE__