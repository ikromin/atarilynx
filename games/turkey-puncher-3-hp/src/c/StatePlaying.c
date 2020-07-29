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

#include <time.h>
#include <stdlib.h>
#include "ABCMusic.h"
#include "TgiExtra.h"
#include "Joystick.h"
#include "State.h"
#include "StatePlaying.h"

/*
 ******************************************************************************
 Main game playing mode code
 ******************************************************************************
 */

#define HAND_EXTENDED_VPOS 70
#define HAND_RESTING_VPOS 88
#define LAST_SCORE_END_VPOS 16
#define LAST_SCORE_START_VPOS 46
#define TURKEY_MAX_SIZE 0x0100
#define TURKEY_MIN_SIZE 0x0000
#define GIBS_END_VPOS 64
#define GIBS_START_VPOS 48
#define TURKEY_OV_MAX_TIME 2
#define MAX_TURKEY_HP 6
#define MAX_SCORE 5000

static char txt[6] = "";
static time_t scorePrevious = 0, scoreCurrent = 0, scoreElapsed = 0;
static u8 lastScorePos = LAST_SCORE_START_VPOS;
static u8 turkeyHP = MAX_TURKEY_HP;
static u8 showTurkeyGibs = 0;
static u8 curPunchStreak = 0;
static u8 maxPunchStreak = 0;
static u8 showPunchStreak = 0;
static u8 showTurkeyOvSprite = 0;

static char* spreeMessages[] = {
	"YOU'RE WEAK",
	"KILLING SPREE",
	"RAMPAGE",
	"DOMINATING",
	"UNSTOPPABLE",
	"GODLIKE"
};

static char* tunePlaying = {
  "X7 O3 I10 V35 T10 R100 H8 K20 "
  "|F3-D3-A,3-D,,3-C,,3-D,,,3-"
  "AF-D-A,-D,,-C,,-D,,,-"
  "F2-D2-A,2-D,,2-C,,2-D,,,2-"
  "_AF-D-A,-D,,-C,,-D,,,-"
  "F-D-A,-D,,-C,,-D,,,-:"
};

static char* tunePunch01 = {
  "X10 O5 I0 V100 T5 R100 H7 K20 "
  "a3"
};

static char* tuneGib01 = {
  "X30 O3 I0 V100 T16 R100 H7 K30 "
  "f5"
};

static char* tuneRespawn01 = {
  "X70 O5 I0 V100 T6 R100 H130 K10 "
  "c'2a2d2"
};

extern char img_bg[];
static SCB_REHV_PAL bgSprite = {
	BPP_4 | TYPE_BACKGROUND, REHV | PACKED, 0x0,
  0x0, &img_bg[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

extern char img_hand1[];
static SCB_REHV handSprite = {
	BPP_4 | TYPE_NORMAL, REHV | PACKED | REUSEPAL, 0x0,
  0x0, &img_hand1[0],
  5, 88,
	0x0100, 0x0100
};

extern char img_turkey1[];
static SCB_REHV turkeySprite = {
	BPP_4 | TYPE_NORMAL, REHV | PACKED | REUSEPAL, 0x0,
  0x0, &img_turkey1[0],
  81, 76,
	TURKEY_MIN_SIZE, TURKEY_MIN_SIZE
};

extern char img_turkeyov1[];
static SCB_REHV turkeyOvSprite = {
	BPP_4 | TYPE_NORMAL, REHV | PACKED | REUSEPAL, 0x0,
  0x0, &img_turkeyov1[0],
  51, 61,
	0x0100, 0x0100
};

extern char img_gibs1[];
static SCB_REHV gibsSprite = {
	BPP_4 | TYPE_NORMAL, REHV | PACKED | REUSEPAL, 0x0,
  0x0, &img_gibs1[0],
  55, GIBS_END_VPOS,
	0x0100, 0x0100
};

void stateChange_Playing() {
	showTurkeyOvSprite = 0;
	(&handSprite)->vpos = HAND_RESTING_VPOS;
  abcplay(0, tunePlaying);
}

void processInput_Playing() {
	BJOY_B; // ignore button B

	if (BJOY_A) {
		scoreCurrent = clock();
		scoreElapsed = scoreCurrent - scorePrevious;
		scorePrevious = scoreCurrent;

		(&handSprite)->vpos = HAND_EXTENDED_VPOS;

		if (scoreElapsed < 10) {
			if (lastScore == 50) curPunchStreak++;
			if (maxPunchStreak < curPunchStreak) maxPunchStreak = curPunchStreak;

			lastScore = 50;
		}
		else {
			if (scoreElapsed < 15) lastScore = 10;
			else if (scoreElapsed < 20) lastScore = 5;
			else lastScore = 1;
			
			curPunchStreak = 0;
		}

		showTurkeyOvSprite = TURKEY_OV_MAX_TIME;
		lastScorePos = LAST_SCORE_START_VPOS;
		score += lastScore;
		abcplay(1, tunePunch01);

		if (score > MAX_SCORE) {
      changeState(GS_ENDGAME);
			return;
		}

		turkeyHP--;

		if (turkeyHP == 0) {
			processInputs = 0;
			showTurkeyGibs = 1;
			showPunchStreak = 1;
			abcplay(1, tuneGib01);
		}
	}
}

void render_Playing() {
	tgi_sprite(&bgSprite);
	
	if (showTurkeyGibs) tgi_sprite(&gibsSprite);
	else {
		tgi_sprite(&turkeySprite);
		if (showTurkeyOvSprite) tgi_sprite(&turkeyOvSprite);
	}

	tgi_sprite(&handSprite);

	tgi_setcolor(1);
	tgi_outtextcentery(3, utoa(score, txt, 10));

	if (lastScore != 0) {
		tgi_setcolor(15);
		tgi_outtextcentery(lastScorePos, utoa(lastScore, txt, 10));
	}

	if (showPunchStreak) {
		tgi_setcolor(0);
		tgi_bar(0, 40, 160, 56);
		tgi_setcolor(13);
		tgi_outtextcentery(45, spreeMessages[maxPunchStreak]);
	}
}

void update_Playing() {
	if ((&handSprite)->vpos < HAND_RESTING_VPOS) {
		(&handSprite)->vpos += 4;
	}

	if (lastScorePos > LAST_SCORE_END_VPOS) {
		lastScorePos -= 6;
	}
	else {
		lastScore = 0;
	}

	// gibs
	if (showTurkeyGibs) {
		if ((&gibsSprite)->vpos == GIBS_END_VPOS && turkeyHP == 0) {
			(&gibsSprite)->vpos = GIBS_START_VPOS;
			
			// reset turkey enter animation
			(&turkeySprite)->hsize = TURKEY_MIN_SIZE;
			(&turkeySprite)->vsize = TURKEY_MIN_SIZE;
		}
		else {
			(&gibsSprite)->vpos += 4;
			if ((&gibsSprite)->vpos == GIBS_END_VPOS) {
				showTurkeyGibs = 0;
				abcplay(1, tuneRespawn01);
			}
		}
	}
	// turkey
	else {
		if ((&turkeySprite)->hsize < TURKEY_MAX_SIZE) {
			(&turkeySprite)->hsize += 0x025;
			(&turkeySprite)->vsize += 0x025;
		}
		else {
			if (processInputs == 0) {
				turkeyHP = MAX_TURKEY_HP;
				curPunchStreak = 0;
				maxPunchStreak = 0;
				showPunchStreak = 0;
				processInputs = 1;
				scorePrevious = clock();
			}
		}
	}

	if (showTurkeyOvSprite) showTurkeyOvSprite--;
}
