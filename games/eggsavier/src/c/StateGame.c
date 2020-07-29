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

#include <stdlib.h>
#include "ABCMusic.h"
#include "TgiExtra.h"
#include "Joystick.h"
#include "State.h"
#include "Font7Seg.h"

/*
 ******************************************************************************
 Logo screen code
 ******************************************************************************
 */

#define MAX_GAME_SPEED 20
#define EGG_CON_LOC_MAX 7
#define COLOR_TEXT 11
#define EGG_EMIT_THRESHOLD 11

extern char img_bg[];
static SCB_REHV_PAL bgSprite = {
	BPP_4 | TYPE_BACKGROUND, REHV | PACKED, 0x0,
  0x0, &img_bg[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

extern char img_bunny[];
static SCB_RENONE bunnySprite = {
	BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0,
  0x0, &img_bunny[0],
  -30, 81
};

extern char img_conveyor1[];
extern char img_conveyor2[];
static SCB_RENONE conveyorSprite = {
	BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0,
  0x0, &img_conveyor1[0],
  5, 5
};
static OBJ_SPR_ANIM conveyor1 = {
	&conveyorSprite, 0, 18, 1, 2, {img_conveyor1, img_conveyor2}
};
static OBJ_SPR_ANIM conveyor2 = {
	&conveyorSprite, 40, 18, 0, 2, {img_conveyor1, img_conveyor2}
};
static OBJ_SPR_ANIM conveyor3 = {
	&conveyorSprite, 81, 18, 1, 2, {img_conveyor1, img_conveyor2}
};
static OBJ_SPR_ANIM conveyor4 = {
	&conveyorSprite, 121, 18, 0, 2, {img_conveyor1, img_conveyor2}
};

extern char img_egg[];
static SCB_RENONE eggSprite = {
	BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0,
  0x0, &img_egg[0],
  145, 4
};

extern char img_sparks[];
static SCB_RENONE sparksSprite = {
	BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0,
  0x0, &img_sparks[0],
  -100, 14
};

extern char img_egg_smashed[];
static SCB_RENONE eggSmashedSprite = {
	BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0,
  0x0, &img_egg_smashed[0],
  -100, 88
};

static char* tuneStep = {
  "X70 O3 I0 V100 T4 R100 H100 K10 "
  "f"
};

static char* tuneScore = {
  "X70 O2 I0 V100 T4 R100 H100 K10 "
  "fc"
};

static char* tuneDead = {
  "X70 O4 I0 V100 T6 R100 H100 K10 "
  "a2 O5T4 f"
};

static u8 gameSpeed, isDead, maxFallingEggs, conveyorEggs, fallingEggs, bunnyPosition, sparksLoc, skipUpdateFrames, i;
static u8 eggLoc[3], fallingEggHeight[3];
static unsigned int speedIncScoreDelta, nextSpeedInc;

static const u8 bunnyLocs[] = {112, 71, 31};
static const u8 convEggLocs[] = {145, 125, 105, 85, 64, 44, 24, 4};
static const u8 pathEggLocs[] = {
	113, 38, 119, 53, 115, 67,
	71, 38, 77, 53, 74, 67,
	31, 38, 35, 53, 34, 67
};
static const u8 convSparkLocs[] = {117, 76, 36};
static const u8 smashedEggLocs[] = {98, 57, 18};
static const u8 gameModeLocs[] = {145, 150, 155};


void stateChange_Game() {
	Joy_Clear();
	tgi_setbgcolor(0);
	tgi_setcolor(COLOR_TEXT);

	gameSpeed = MAX_GAME_SPEED;
	isDead = 0;
	conveyorEggs = 0;
	maxFallingEggs = gameMode + 1;
	fallingEggs = 0;
	bunnyPosition = 1;
	sparksLoc = 255;
	skipUpdateFrames = MAX_GAME_SPEED;
	speedIncScoreDelta = 5;
	nextSpeedInc = 5;
	score = 0;

	for (i = 0; i < 3; i++) {
		eggLoc[i] = 255;
		fallingEggHeight[i] = 255;
	}
}

void processInput_Game() {
	if (!isDead) {
		if (BJOY_RIGHT && bunnyPosition > 0) bunnyPosition--;
		if (BJOY_LEFT && bunnyPosition < 2) bunnyPosition++;
	}
	else {
		if (BJOY_A) stateChange_Game();
		if (BJOY_B) changeState(GS_LOGO);
	}
	Joy_Clear();
}

static void __fastcall__ updateAnimation(OBJ_SPR_ANIM* obj) {
	obj->currentFrame++;
	if (obj->currentFrame == obj->totalFrames) obj->currentFrame = 0;
}

static void __fastcall__ startEggFalling(u8 pathNum, u8 eggNum) {
	eggLoc[eggNum] = 255;
	sparksLoc = 255;
	conveyorEggs--;
	fallingEggHeight[pathNum] = 0;
	fallingEggs++;
}

void update_Game() {
	if (isDead) return;

	// animation logic

	if (skipUpdateFrames) skipUpdateFrames--;
	if (skipUpdateFrames > 0) return;
	skipUpdateFrames = gameSpeed;

	updateAnimation(&conveyor1);
	updateAnimation(&conveyor2);
	updateAnimation(&conveyor3);
	updateAnimation(&conveyor4);

	// gameplay logic

	// advance each of the eggs on the conveyor belt
	for (i = 0; i < maxFallingEggs; i++) {
		if (eggLoc[i] != 255) {
			eggLoc[i]++;
			if (eggLoc[i] > EGG_CON_LOC_MAX) {
				eggLoc[i] = 255;
				conveyorEggs--;
			}
		}
	}

	// emit an egg randomly into the first spot if there's room on the conveyor
	if (conveyorEggs < maxFallingEggs) {
		for (i = 0; i < maxFallingEggs; i++) {
			if (eggLoc[i] == 255) {
				if (((rand() & 0xf) + 1) <= EGG_EMIT_THRESHOLD) {
					eggLoc[i] = 0;
					conveyorEggs++;
					break;
				}
			}
		}
	}

	// advance each of the falling eggs
	for (i = 0; i < 3; i++) { 
		if (fallingEggHeight[i] != 255) {
			fallingEggHeight[i]++;
			if (fallingEggHeight[i] > 2) { // end of path - either caught or smashed
				fallingEggs--;
				fallingEggHeight[i] = 255;

				if (bunnyPosition == i) {
						abcplay(2, tuneScore);
						score++;
						if (nextSpeedInc == score && gameSpeed > 0) {
							gameSpeed--;
							nextSpeedInc += speedIncScoreDelta;
							speedIncScoreDelta += 3;
						}
					}
					else {
						isDead = i + 1;
						abcplay(2, tuneDead);
					}
			}
		}
	}

	// check if an egg should start falling on any specific path
	if (sparksLoc != 255) {
		for (i = 0; i < maxFallingEggs; i++) {
			u8 eggLocI = eggLoc[i];
			if      (eggLocI == 2 && sparksLoc == 0) { startEggFalling(0, i); }
			else if (eggLocI == 4 && sparksLoc == 1) { startEggFalling(1, i); }
			else if (eggLocI == 6 && sparksLoc == 2) { startEggFalling(2, i); }
		}
	}

	// randomly place a spark
	if (fallingEggs < maxFallingEggs && sparksLoc == 255 && ((rand() & 0xf) + 1) <= gameSpeed) {
		u8 p = (rand() & 0xff) + 1;
		if (p < 85 && fallingEggHeight[0] == 255) sparksLoc = 0;
		else if (p > 170 && fallingEggHeight[1] == 255) sparksLoc = 1;
		else if(fallingEggHeight[2] == 255) sparksLoc = 2;
		else sparksLoc = 255;
	}

	abcplay(1, tuneStep);
}

void render_Game() {
	SCB_RENONE* sprite = &eggSprite;

	tgi_sprite(&bgSprite);

	// draw conveyor belts
	drawAnimSprite(&conveyor1);
	drawAnimSprite(&conveyor2);
	drawAnimSprite(&conveyor3);
	drawAnimSprite(&conveyor4);

	// draw bunny
	(&bunnySprite)->hpos = bunnyLocs[bunnyPosition];
	tgi_sprite(&bunnySprite);

	// draw sparks
	if (sparksLoc != 255) {
		(&sparksSprite)->hpos = convSparkLocs[sparksLoc];
		tgi_sprite(&sparksSprite);
	}

	// draw eggs on conveyor belt
	for (i = 0; i < maxFallingEggs; i++) {
		if (eggLoc[i] != 255) {
			sprite->hpos = convEggLocs[eggLoc[i]];
			sprite->vpos = 4;
			tgi_sprite(sprite);
		}
	}

	// draw falling egg paths
	if (!isDead) {
		for (i = 0; i < 3; i++) {
			if (fallingEggHeight[i] != 255) {
				u8 idx = (6 * i) + (fallingEggHeight[i] * 2);
				sprite->hpos = pathEggLocs[idx];
				sprite->vpos = pathEggLocs[idx + 1];
				tgi_sprite(sprite);
			}
		}
	}

	// draw level and score
	draw7SegNumber(7, 39, score);
	draw7SegNumber(148, 39, (MAX_GAME_SPEED - gameSpeed) + 1);

	if (isDead) {
		sprite = &eggSmashedSprite;
		sprite->hpos = smashedEggLocs[isDead - 1];
		tgi_sprite(sprite);

		tgi_outtextcentery(45, "GAME OVER!!");
		tgi_outtextcentery(58, "A - RESTART");
		tgi_outtextcentery(68, "B - MENU   ");
	}

	i = gameModeLocs[gameMode];
	tgi_line(i, 93, i + 2, 93);
}
