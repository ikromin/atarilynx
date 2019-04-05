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
#include "TgiExtra.h"
#include "Joystick.h"
#include "State.h"
#include "StateLogo.h"

/*
 ******************************************************************************
 Logo screen code
 ******************************************************************************
 */

static char* tuneLogo = {
  "X70 I0 V65 T8 R100 K20 "
  "|"
  "O2 H10 "
  "E,,B,,,-B,,3- B,,2-E,,2B,,2-B,2-E,2B,2-E,E,2"
  "O3 H15 "
  "E'B'-B'3- B'2-E'2B'2-B2-E2B2-EE2"
  ":"
};

extern char img_logo[];
static SCB_REHV_PAL logoSprite = {
	BPP_4 | TYPE_BACKGROUND, REHV | PACKED, 0x0,
  0x0, &img_logo[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

#define START_COLOUR_1 1
#define START_COLOUR_2 3
static u8 startColour = START_COLOUR_1;

void stateChange_Logo() {
	abcstop();
	abcplay(0, tuneLogo);
}

void processInput_Logo() {
	BJOY_B; // ignore button B
	if (BJOY_A) changeState(GS_PSYCHED);
}

void update_Logo() {
	startColour = (startColour == START_COLOUR_1) ? START_COLOUR_2 : START_COLOUR_1;
}

void render_Logo() {
	tgi_sprite(&logoSprite);
	tgi_setcolor(startColour);
	tgi_outtextcentery(85, "A TO START");
}

