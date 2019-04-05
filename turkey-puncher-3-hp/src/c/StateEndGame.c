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
#include "StateEndGame.h"

/*
 ******************************************************************************
 End Game code
 ******************************************************************************
 */

static char* tuneEndGame = {
  "X7 O4 I10 V85 T10 R100 H40 K30 "
  "|"
  "E_AB_AeB_AB_dB_AE_AAB_ABAB_dA_A"
  ":"
};

extern char img_ag[];
static SCB_REHV_PAL agSprite = {
	BPP_4 | TYPE_BACKGROUND, REHV | PACKED, 0x0,
  0x0, &img_ag[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

void stateChange_EndGame() {
	abcplay(0, tuneEndGame);
}

void processInput_EndGame() {
	BJOY_A; // ignore button B
	if (BJOY_B) changeState(GS_PSYCHED);
}

void render_EndGame() {
	tgi_sprite(&agSprite);
}

void update_EndGame() {}