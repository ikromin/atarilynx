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

#include "ABCMusic.h"
#include "TgiExtra.h"
#include "Joystick.h"
#include "State.h"

/*
 ******************************************************************************
 Logo screen code
 ******************************************************************************
 */

extern char img_logo[];
static SCB_REHV_PAL logoSprite = {
	BPP_4 | TYPE_BACKGROUND, REHV | PACKED, 0x0,
  0x0, &img_logo[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

#define COLOR_TEXT_OPT 2
#define COLOR_TEXT_MODE 11

static char* tuneIntro = {
  "X70 O2 I0 V100 T12 R100 H100 K10 "
  "|gdBdgdgdgd_Bdgdgdf_d_B_df_df_dec_Bcgcgc:"
};

static char* gameModes[] = {"GAME A", "GAME B", "GAME C", "INFORMATION"};


void stateChange_Logo() {
	abcstop();
	abcplay(1, tuneIntro);
}

void processInput_Logo() {
	if (BJOY_A) {
		if (gameMode == 3) changeState(GS_HELP);
		else changeState(GS_GAME);
	}
	if (BJOY_B) {
		gameMode++;
		if (gameMode > 3) gameMode = 0;
	}
}

void update_Logo() {
}

void render_Logo() {
	tgi_sprite(&logoSprite);
	
	tgi_setcolor(COLOR_TEXT_OPT);
	tgi_outtextcentery(83, "A - START ");
	tgi_outtextcentery(93, "B - CHANGE");

	tgi_setcolor(COLOR_TEXT_MODE);
	tgi_outtextcentery(72, gameModes[gameMode]);
}

