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
 Logo screen code
 ******************************************************************************
 */

#define MAX_ANIM_FRAME 15
#define MAX_ANIM_STAGE 3
#define MAX_CONT_COLOUR 5

extern char img_logo[];
static SCB_REHV_PAL logoSprite = {
	BPP_3 | TYPE_NORMAL, REHV | PACKED, 0x0,
  0x0, &img_logo[0],
  0, 0, // offset from scale e.g. -160 / (scale/256)
	0x0100, 0x0100, // 2x scale e.g. 512
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

// pre-calculated scale and position tables
static const u16 scaledSize[] = {0x100, 0x180, 0x200, 0x280, 0x300, 0x380, 0x400, 0x480, 0x500, 0x580, 0x600, 0x680, 0x700, 0x780, 0x800, 0x880};
static const u16 scaledPosOffX[] = {25, -3, -30, -58, -85, -113, -140, -168, -195, -223, -250, -278, -305, -333, -360, -388};
static const u16 scaledPosOffY[] = {21, 6, -9, -25, -39, -55, -69, -85, -99, -115, -129, -145, -159, -175, -189, -205};

static u8 animStage = 0;
static u8 skipFrame = MAX_ANIM_FRAME;
static u8 animFrame = MAX_ANIM_FRAME;

void processInput_Logo() {
	if (BJOY_A || BJOY_B) {
		changeState(GS_SELECT);
	}
}

void update_Logo() {
	if (animStage < MAX_ANIM_STAGE) {
		if (skipFrame > 0) {
			skipFrame--;
		}
		else {
			animStage++;
			skipFrame = MAX_ANIM_FRAME;
		}
	}
	if (animStage == 0) {
		animFrame = skipFrame;
	}

	continueColour++;
	if (continueColour > MAX_CONT_COLOUR) {
		continueColour = 0;
	}
}

void render_Logo() {
	tgi_clear();

	(&logoSprite)->hsize = scaledSize[animFrame];
	(&logoSprite)->vsize = scaledSize[animFrame];
	(&logoSprite)->hpos  = scaledPosOffX[animFrame];
	(&logoSprite)->vpos  = scaledPosOffY[animFrame];
	tgi_sprite(&logoSprite);

	// animStage 1 is just a delay

	tgi_setcolor(1);

	if (animStage >= 2) {
		tgi_outtextcentery(7, "Atari Gamer");
	}

	if (animStage >= 3) {
		tgi_outtextcentery(78, "Lynx 30th Birthday");

		tgi_setcolor(contColourArr[continueColour]);
		tgi_outtextcentery(90, "Press A or B");
	}

}

