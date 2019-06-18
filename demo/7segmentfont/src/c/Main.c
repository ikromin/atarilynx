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

#include <stdlib.h>
#include <time.h>
#include <6502.h>
#include "Types.h"
#include "Font7Seg.h"
#include "Palette.h"


void main(void)  {
	// install TGI driver
	tgi_install(tgi_static_stddrv);
	tgi_setframerate(60);
	tgi_init();
	CLI();

	tgi_setpalette(palette);
	tgi_setbgcolor(0);
	
	tgi_setcolor(1);
	tgi_bar(0, 0, 159, 101);

	tgi_setcolor(2);
	tgi_outtextxy(3, 3, "7-Segment Demo");
	

	set7SegColor(2);
	draw7SegNumber(3, 20, 7, 0, 1);
	set7SegColor(3);
	draw7SegNumber(3, 30, 7, 0, 12);
	set7SegColor(4);
	draw7SegNumber(3, 40, 7, 0, 123);

	set7SegColor(5);
	draw7SegNumber(40, 20, 0, 10, 1234);
	set7SegColor(6);
	draw7SegNumber(50, 20, 0, 10, 12345);
	set7SegColor(7);
	draw7SegNumber(60, 20, 0, 10, 65535);

	set7SegColor(8);
	draw7SegNumber(80, 20, 8, 10, 123);
	set7SegColor(9);
	draw7SegNumber(90, 20, 8, 10, 456);
	set7SegColor(10);
	draw7SegNumber(100, 20, 8, 10, 789);
	set7SegColor(11);
	draw7SegNumber(110, 20, 8, 10, 0);

	while (tgi_busy());
	tgi_updatedisplay();
}
