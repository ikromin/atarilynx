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
#define MAX_SELECTED_GAME 10
#define ARROW_OFFSET 50

static u8 arrowColourIdx = 14;

void processInput_Select() {
  if (BJOY_LEFT)  selectedGame--;
	if (BJOY_RIGHT) selectedGame++;

  if (selectedGame < 0) selectedGame = MAX_SELECTED_GAME;
  if (selectedGame > MAX_SELECTED_GAME) selectedGame = 0;

	BJOY_B; // ignore B button presses
	if (BJOY_A) {
		changeState(GS_RUN);
	}
}

void update_Select() {
	arrowColourIdx++;
  if (arrowColourIdx > 15) {
    arrowColourIdx = 0;
  }
}

void render_Select() {
  tgi_clear();

  (&logoGame)->data = gameImages[selectedGame];

  tgi_sprite(&logoGame);

  tgi_setcolor(arrowColourIdx);
  tgi_outtextxy(5, ARROW_OFFSET, "<");
  tgi_outtextxy(159 - 5 - 8, ARROW_OFFSET, ">");

  tgi_setcolor(6);
  tgi_outtextcentery(90, gameNames[selectedGame]);
}
