#include "AusDay.h"
#include "Pallete.h"

int barColour = BAR_COLOUR_1;
int delay = FLICK_FRAMES;

char* tune01 = {
	"X7 O2 I1 V100 T12 R0 H8 K4 z6 "
	"G2 c2 G2 E2 G2 c3 c c2 z"
	"e2 d2 c2 B2 c2 d4 z"
	"G2 c2 G2 E2 C2 G3 G G2 e2 d2 c2 B2 A2 G4 z"
	"G2 A3 B c2 A2 G3 E E2 G2 A2 c2 e2 f2 d4 z"
	"G2 A2 B c2 A2 G3 c c2 d2 e3 c d3 B"
	"c6 e2 f2 e2 d2 c2 B2 A2 G3 cc e3 c d3 B c4 z"
};

SCB_REHV_PAL iconsprite =  {
	BPP_4 | TYPE_NORMAL,
	REHV,
	0,
	0,
	&flag[0],
  0, // x
	11, // y
	0x0100, // x scale
	0x0100, // y scale
	{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }  
};

void initialize()
{
	tgi_install(&lynxtgi);
	tgi_setframerate(60);

	tgi_init();
	CLI();
	while (tgi_busy()) {};

	setpalette(palette);
	tgi_setbgcolor(0);
	tgi_clear();

	abcstop();
}

void main(void) 
{	
	initialize();	

	while (1)
	{
		// play the tune if it's not playing already
		if (abcactive[0] == 0) {
			abcplay(0, tune01);
		}

		if (!tgi_busy()) {
		  // draw flag sprite
			tgi_sprite(&iconsprite);
	
			// draw flickering colour bars
			tgi_setcolor(barColour);
			tgi_bar(0, 0, 160, 10);
			tgi_bar (0, 91, 160, 102);

			// very crude timing code that flips colours
			// every FLICK_FRAMES number of frames
			delay--;
			if (delay == 0) {
				if (barColour == BAR_COLOUR_1) {
					barColour = BAR_COLOUR_2;
				}
				else {
					barColour = BAR_COLOUR_1;
				}
			
				delay = FLICK_FRAMES;
			}
	
			tgi_updatedisplay();
		}
	};
}
