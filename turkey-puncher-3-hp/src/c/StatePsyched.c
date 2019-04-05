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

#include "ABCMusic.h"
#include "Joystick.h"
#include "TgiExtra.h"
#include "State.h"
#include "StatePsyched.h"

/*
 ******************************************************************************
 Get Psyched screen code
 ******************************************************************************
 */

void stateChange_Psyched() {
  processInputs = 20;
  abcstop();
  score = 0;
  lastScore = 0;
}

void processInput_Psyched() {
  BJOY_A; // ignore button A
  BJOY_B; // ignore button B
}

void render_Psyched() {
	tgi_clear();
	tgi_setcolor(15);
	tgi_outtextcentery(46, "GET PSYCHED!");
}

void update_Psyched() {
	processInputs--;
	if (processInputs == 0) changeState(GS_PLAYING);
}
