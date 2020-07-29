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

#ifndef __LYNX_PALETTE__
#define __LYNX_PALETTE__

// https://atarigamer.com/pages/atari-lynx-palette-generator
// 330000, cd0001, 313331, 626462, 8d8d8b, dddfdc, ffff00, 00ffff, 050505, 252525, 4b4b4b, 656565, 838383, a1a1a1, bebebe, f4f4f4
u8 palette[] = {
  // green
	0x0003 >> 8, 
	0x000c >> 8, 
	0x0333 >> 8, 
	0x0666 >> 8, 
	0x0989 >> 8, 
	0x0edd >> 8, 
	0x0f0f >> 8, 
	0x0ff0 >> 8, 
	0x0000 >> 8, 
	0x0222 >> 8, 
	0x0555 >> 8, 
	0x0666 >> 8, 
	0x0888 >> 8, 
	0x0aaa >> 8, 
	0x0ccc >> 8, 
	0x0fff >> 8, 

	// blue + red
	0x0003 & 0xff, 
	0x000c & 0xff, 
	0x0333 & 0xff, 
	0x0666 & 0xff, 
	0x0989 & 0xff, 
	0x0edd & 0xff, 
	0x0f0f & 0xff, 
	0x0ff0 & 0xff, 
	0x0000 & 0xff, 
	0x0222 & 0xff, 
	0x0555 & 0xff, 
	0x0666 & 0xff, 
	0x0888 & 0xff, 
	0x0aaa & 0xff, 
	0x0ccc & 0xff, 
	0x0fff & 0xff
};

#endif // __LYNX_PALETTE__