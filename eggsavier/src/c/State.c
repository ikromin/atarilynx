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

#include "State.h"

/*
 ******************************************************************************
 Global game state variables and function pointer code for state switching.
 ******************************************************************************
 */

u8 processInputs = 1;
unsigned int score = 0;
u8 gameMode = 0;

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
    
    case GS_GAME:
      stateChange_Game();
      _processInput = &processInput_Game;
      _render = &render_Game;
      _update = &update_Game;
      break;

    case GS_HELP:
      stateChange_Help();
      _processInput = &processInput_Help;
      _render = &render_Help;
      _update = &update_Help;
      break;
  }
}