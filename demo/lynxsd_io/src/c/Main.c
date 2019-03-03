#include "Main.h"
#include "Types.h"
#include "LynxSD.h"

/*
 ******************************************************************************
 A demo showing reading and writing data from/to the Lynx SD cart
 ******************************************************************************
 */


u8 colourIndex = 0;


// pallete colours in 0x0GBR format, 4 rows of 4 colours each
u8 palette[] = {
  // green values
  0x0f00 >> 8,    0x00f0 >> 8,    0x000f >> 8,    0x0ff0 >> 8,
  0x00ff >> 8,    0x0f0f >> 8,    0x0fff >> 8,    0x0222 >> 8,
  0x0900 >> 8,    0x0090 >> 8,    0x0009 >> 8,    0x0990 >> 8,
  0x0099 >> 8,    0x0909 >> 8,    0x0999 >> 8,    0x0444 >> 8,
  // blue and red values
  0x0f00 & 0xff,  0x00f0 & 0xff,  0x000f & 0xff,  0x0ff0 & 0xff,	
  0x00ff & 0xff,  0x0f0f & 0xff,  0x0fff & 0xff,  0x0222 & 0xff,
  0x0900 & 0xff,  0x0090 & 0xff,  0x0009 & 0xff,  0x0990 & 0xff,	
  0x0099 & 0xff,  0x0909 & 0xff,  0x0999 & 0xff,  0x0444 & 0xff
};


void main(void) 
{
	// install TGI driver
	tgi_install(&lynxtgi);
	tgi_setframerate(60);
	tgi_init();
	CLI();

	while (tgi_busy());
	tgi_setpalette(palette);
	tgi_setbgcolor(15);
	tgi_setcolor(1);

	joy_install(&lynxjoy);

	// try to read a state save file if it exists
	if (LynxSD_OpenFile("/saves/state.sav") == FR_OK) {
		LynxSD_ReadFile(&colourIndex, 1);
		LynxSD_CloseFile();
	}

	while (1) {
		u8 joyState = joy_read(JOY_1);

		// Button A pressed, change colour index
		if (joyState & JOY_BTN_1_MASK) {
			colourIndex++;
			if (colourIndex == 15) colourIndex = 0;
		}

		// Button B pressed, write index to file
		if (joyState & JOY_BTN_2_MASK) {
			if (LynxSD_OpenFile("/saves/state.sav") == FR_OK) {
				LynxSD_WriteFile(&colourIndex, 1);
				LynxSD_CloseFile();
			}
		}
		
		// draw a full screen rectange of the selected colour
		if (!tgi_busy()) {
			tgi_clear();
			tgi_setcolor(colourIndex);
			tgi_bar(0, 0, 159, 101);
			tgi_updatedisplay();
		}
	}
}
