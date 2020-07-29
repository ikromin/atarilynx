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

#include <stdlib.h>
#include <6502.h>
#include "Types.h"
#include "Joystick.h"
#include "State.h"
#include "Palette.h"

#define TIME_PER_UPDATE 3

void main(void)  {
	// initialise all game loop times to zero
	_engTimePrevious = clock();
	_engTimeCurrent = 0;
	_engTimeElapsed = 0;
	_engTimeLag = 0;

	// reset joystick states
	_engProcessInputs = 1;
	_engJoyState = 0;
	_engPreviousJoyState = 0;

	// install TGI driver
	tgi_install(tgi_static_stddrv);
	tgi_setframerate(60);
	tgi_init();
	CLI();

	tgi_setpalette(palette);
	tgi_setbgcolor(0);
	tgi_clear();

	joy_install(lynx_stdjoy_joy);

	changeState(GS_LOGO);

	while (1) {
		_engTimeCurrent = clock();
		_engTimeElapsed = _engTimeCurrent - _engTimePrevious;
		_engTimePrevious = _engTimeCurrent;
		_engTimeLag += _engTimeElapsed;

		if (_engProcessInputs != 0) {
			Joy_Buffer();
			_engFuncProcessInput();
		}

		while (_engTimeLag >= TIME_PER_UPDATE) {
			_engFuncUpdate();
			_engTimeLag -= TIME_PER_UPDATE;
		}

		// draw a frame if TGI is ready
		if (!tgi_busy()) {
			_engFuncRender();
			tgi_updatedisplay();
		}
	}

}