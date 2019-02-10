#include "Main.h"

/*
 ******************************************************************************
 A demo showing input processing for joystick events with buffering
 ******************************************************************************
 */


#define COLOR_ON 1
#define COLOR_OFF 2


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

// joystick states
#define BJOY_UP joy(JOY_UP_MASK)
#define BJOY_DOWN joy(JOY_DOWN_MASK)
#define BJOY_LEFT joy(JOY_LEFT_MASK)
#define BJOY_RIGHT joy(JOY_RIGHT_MASK)
#define BJOY_A joy(JOY_BTN_1_MASK)
#define BJOY_B joy(JOY_BTN_2_MASK)
unsigned char joyState = 0;
unsigned char __fastcall__ joy(unsigned char mask) {
	if ((joyState & mask) == 0) { // if button is not pressed, do nothing
		return 0;
	}
	joyState ^= mask; // reset buffered button state
	return 1;
}


void drawScreen() {
	tgi_clear();

	tgi_setcolor(BJOY_UP ? COLOR_ON : COLOR_OFF);
	tgi_bar (40, 20,			// up joy button
						60, 40);
	
	tgi_setcolor(BJOY_DOWN ? COLOR_ON : COLOR_OFF);
	tgi_bar (40, 62,			// down joy button
						60, 82);
	
	tgi_setcolor(BJOY_LEFT ? COLOR_ON : COLOR_OFF);
	tgi_bar (19, 41,			// left joy button
						39, 61);

	tgi_setcolor(BJOY_RIGHT ? COLOR_ON : COLOR_OFF);
	tgi_bar (61, 41,			// right joy button
						81, 61);

	tgi_setcolor(BJOY_B ? COLOR_ON : COLOR_OFF);
	tgi_bar (90, 20,			// B button
						110, 40);
	
	tgi_setcolor(BJOY_A ? COLOR_ON : COLOR_OFF);
	tgi_bar (120, 20,			// A button
						140, 40);

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

	// install joystick driver
	joy_install(&lynxjoy);

	while (tgi_busy());
	tgi_setpalette(masterpal);
	tgi_setbgcolor(0);
	tgi_setcolor(COLOR_OFF);

	while (1) {
		// buffer joystick state
		joyState |= joy_read(JOY_1);

		// draw the screen if possible
		if (!tgi_busy()) {
			drawScreen();
		}
	}
}
