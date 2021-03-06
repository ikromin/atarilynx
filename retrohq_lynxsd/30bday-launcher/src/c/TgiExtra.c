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

#include <string.h>
#include "TgiExtra.h"

/*
 ******************************************************************************
 Useful, additional TGI functionality
 ******************************************************************************
 */

void __fastcall__ tgi_outtextcentery(u8 y, char* t) {
  tgi_outtextxy((160 - (strlen(t) * 8)) / 2, y, t);
}