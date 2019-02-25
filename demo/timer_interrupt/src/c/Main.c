#include "Main.h"

/*
 ******************************************************************************
 A demo showing input processing for joystick events with buffering
 ******************************************************************************
 */


extern unsigned char vblcount;


// pallete colours in 0x0GBR format, 4 rows of 4 colours each
unsigned char masterpal[] = {
  // green values
  0x0000 >> 8,    0x000b >> 8,    0x0333 >> 8,    0x0444 >> 8,
  0x0999 >> 8,    0x0115 >> 8,    0x0aaa >> 8,    0x0aaa >> 8,
  0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,
  0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,
  // blue and red values
  0x0000 & 0xff,  0x000b & 0xff,  0x0333 & 0xff,  0x0444 & 0xff,	
  0x0999 & 0xff,  0x0115 & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,
  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,	
  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff
};


void drawScreen() {
	char buf[4];

	tgi_clear();

	tgi_outtextxy(3, 3,
		itoa(vblcount, buf, 10)
		);

	tgi_updatedisplay();
}


void main(void) 
{
	// clear hardware pallete to start with a blank screen
	unsigned char* ptr = 0xfda0;
	unsigned char count = 0x20;
	while (count--) {
		*ptr++ = 0;
	}

	// install TGI driver
	tgi_install(&lynxtgi);
	tgi_setframerate(60);
	tgi_init();
	CLI();

	while (tgi_busy());
	tgi_setpalette(masterpal);
	tgi_setbgcolor(0);
	tgi_setcolor(1);

	while (1) {
		// draw the screen if possible
		if (!tgi_busy()) {
			drawScreen();
		}
	}
}
