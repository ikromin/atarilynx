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

#ifndef __LYNX_PALETTE__
#define __LYNX_PALETTE__

static u8 palette[] = {
  // green
  0x000 >> 8, 
  0x0007 >> 8, 
  0x0212 >> 8, 
  0x000b >> 8, 
  0x0434 >> 8, 
  0x0325 >> 8, 
  0x0424 >> 8, 
  0x0339 >> 8, 
  0x0546 >> 8, 
  0x0538 >> 8, 
  0x050c >> 8, 
  0x0778 >> 8, 
  0x095b >> 8, 
  0x0bb9 >> 8, 
  0x0bbb >> 8, 
  0x0fff >> 8, 
  
  // blue + red
  0x000 & 0xff, 
  0x0007 & 0xff, 
  0x0212 & 0xff, 
  0x000b & 0xff, 
  0x0434 & 0xff, 
  0x0325 & 0xff, 
  0x0424 & 0xff, 
  0x0339 & 0xff, 
  0x0546 & 0xff, 
  0x0538 & 0xff, 
  0x050c & 0xff, 
  0x0778 & 0xff, 
  0x095b & 0xff, 
  0x0bb9 & 0xff, 
  0x0bbb & 0xff, 
  0x0fff & 0xff
};

#endif // __LYNX_PALETTE__