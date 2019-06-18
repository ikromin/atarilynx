/******************************************************************************
 7-Segment Font Demo

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

// https://atarigamer.com/pages/atari-lynx-palette-generator
// 000000,404040,eb2d2e,a4764f,6b6b6b,2aa0e2,62d76b,bfbfbf,a6de59,a1ede3,fff965,111111,ffffff,ffffff,ffffff,ffffff
static u8 palette[] = {
  // green
	0x0000 >> 8, 
	0x0444 >> 8, 
	0x033e >> 8, 
	0x075a >> 8, 
	0x0666 >> 8, 
	0x0ae3 >> 8, 
	0x0d66 >> 8, 
	0x0ccc >> 8, 
	0x0d5a >> 8, 
	0x0eea >> 8, 
	0x0f6f >> 8, 
	0x0111 >> 8, 
	0x0fff >> 8, 
	0x0fff >> 8, 
	0x0fff >> 8, 
	0x0fff >> 8, 

	// blue + red
	0x0000 & 0xff, 
	0x0444 & 0xff, 
	0x033e & 0xff, 
	0x075a & 0xff, 
	0x0666 & 0xff, 
	0x0ae3 & 0xff, 
	0x0d66 & 0xff, 
	0x0ccc & 0xff, 
	0x0d5a & 0xff, 
	0x0eea & 0xff, 
	0x0f6f & 0xff, 
	0x0111 & 0xff, 
	0x0fff & 0xff, 
	0x0fff & 0xff, 
	0x0fff & 0xff, 
	0x0fff & 0xff
};

#endif // __LYNX_PALETTE__