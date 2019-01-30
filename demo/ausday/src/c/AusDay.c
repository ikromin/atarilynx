#include <stdlib.h>
#include <6502.h>
#include <lynx.h>
#include <tgi.h>
#include <peekpoke.h>
#include <string.h>
#include <conio.h>

extern char lynxtgi[];
extern char flag[];

extern void abcstop();
extern void abcplay(unsigned char channel, char *tune);
extern int abcval;

#define TEMPO "T6"
char* tune01 = {
"X7 O2 I1 V100 T12 R0 H8 K4 z6 "
"G2 c2 G2 E2 G2 c3 c c2 z"
"e2 d2 c2 B2 c2 d4 z"
"G2 c2 G2 E2 C2 G3 G G2 e2 d2 c2 B2 A2 G4 z"
"G2 A3 B c2 A2 G3 E E2 G2 A2 c2 e2 f2 d4 z"
"G2 A2 B c2 A2 G3 c c2 d2 e3 c d3 B"
"c6"
"e2"
"f2"
"e2"
"d2"
"c2"
"B2"
"A2"
"G3"
"cc"
"e3"
"c"
"d3"
"B"
"c4 z"
};


static int palette[] =  {
  0x0fff, 0x011f, 0x0692, 0x0946, 0x0404, 0x0818,
  0x0666, 0x0888, 0x0616, 0x0b7f, 0x066e, 0x0b3b, 
  0x0513, 0x0f0f, 0x0f00, 0x0000
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

void setpalette(const int* palette)
{
  char index;
  for (index = 0; index < 0x10; index++)
  {
    POKE(0xFDA0 + index, palette[index] >> 8);
    POKE(0xFDB0 + index, palette[index] & 0xFF);
  }
}

void initialize()
{
	tgi_install(&lynxtgi);
	tgi_init();
	CLI();
	
	abcstop();
	abcplay(0, tune01);
  
	while (tgi_busy()) {};

	setpalette(palette);

	tgi_setcolor(9);
	tgi_setbgcolor(0); 
	tgi_clear();
}

void main(void) 
{	
	int cInd = 13;
	int delay = 20;
	
	initialize();
	
	while (1)
	{
		if (!tgi_busy()) {
		  	tgi_sprite(&iconsprite);
	
			tgi_setcolor(cInd);
			tgi_bar(0, 0, 160, 10);
			tgi_bar (0, 91, 160, 102);
			
			delay--;
			
			if (delay == 0) {
				if (cInd == 13) {
					cInd = 14;
				}
				else {
					cInd = 13;
				}
			
				delay = 20;
			}
	
			tgi_updatedisplay();
		}
	};
}


