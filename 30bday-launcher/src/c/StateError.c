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

#include "TgiExtra.h"
#include "Joystick.h"
#include "State.h"

/*
 ******************************************************************************
 Error screen code
 ******************************************************************************
 */

#define MAX_CONT_COLOUR 5

void processInput_Error() {
	if (BJOY_A || BJOY_B) {
		changeState(GS_SELECT);
	}
}

void update_Error() {
	continueColour++;
	if (continueColour > MAX_CONT_COLOUR) {
		continueColour = 0;
	}
}

void render_Error() {
	tgi_clear();

	tgi_setcolor(1);
	tgi_outtextcentery(43, "Error loading ROM");

	tgi_setcolor(contColourArr[continueColour]);
	tgi_outtextcentery(90, "Press A or B");
}

