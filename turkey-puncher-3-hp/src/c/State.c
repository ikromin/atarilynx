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

#include "State.h"

/*
 ******************************************************************************
 Global game state variables and function pointer code for state switching.
 ******************************************************************************
 */

u8 processInputs = 1;
u16 score = 0;
u8 lastScore = 0;

void (*_processInput)(void);
void (*_render)(void);
void (*_update)(void);

void __fastcall__ changeState(GS_STATE newState) {
  switch (newState) {
    case GS_LOGO:
      stateChange_Logo();
      _processInput = &processInput_Logo;
      _render = &render_Logo;
      _update = &update_Logo;
      break;

    case GS_PSYCHED:
      stateChange_Psyched();
      _processInput = &processInput_Psyched;
      _render = &render_Psyched;
      _update = &update_Psyched;
      break;

    case GS_PLAYING:
      stateChange_Playing();
      _processInput = &processInput_Playing;
      _render = &render_Playing;
      _update = &update_Playing;
      break;

    case GS_ENDGAME:
      stateChange_EndGame();
      _processInput = &processInput_EndGame;
      _render = &render_EndGame;
      _update = &update_EndGame;
      break;
  }
}