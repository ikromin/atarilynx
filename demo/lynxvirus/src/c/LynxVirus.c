#include "LynxVirus.h"
#include "Pallete.h"

unsigned int msgDelay = 100;
unsigned int delay = 0;
unsigned int stage = 0;
unsigned int randomNum = 0;

char* tune01 = {
	"X10 O6 I0 V100 T2 R0 H1 K1 "
	"|"
	"gebbebbGebbebbz5"
	"gbaz5"
	"eeadadddddadddddddgggggz5"
	"abcdefgz5"
	"DGBeeDBGDeBGEDeBGez"
	":"
};

unsigned int randomNums[] = {
	11,	9,	5,	10,	19,	6,	20,	2,	7,	1,	9,	15,	17,	5,	16,	15,	2,	10,	6,	14
};
unsigned int randIdx = 0;

SCB_REHV_PAL iconsprite0 =  {
	BPP_4 | TYPE_BACKGROUND,
	REHV,
	0,
	0,
	&noinfect[0],
  	0, // x
	0, // y
	0x0100, // x scale
	0x0100, // y scale
	{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }  
};

SCB_REHV_PAL iconsprite1 =  {
	BPP_4 | TYPE_BACKGROUND,
	REHV,
	0,
	0,
	&lynxvirus[0],
  	0, // x
	0, // y
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
		randIdx++;
		if (randIdx > 19) randIdx = 0;
		randomNum = randomNums[randIdx];

		if (!tgi_busy()) {
			delay++;
			if (delay > msgDelay) {
				delay = 0;

				if (stage == 0) {
					abcplay(0, tune01);
				}

				stage++;
			}

			tgi_setcolor(0);
			tgi_sprite(&iconsprite1);

			switch (stage) {
				case 0:
					tgi_sprite(&iconsprite0);
					break;

				case 1:
					tgi_bar(4, 4, 159 - 4, 101 - 4);
					break;

				case 2:
					tgi_bar(4, 35, 159 - 4, 101 - 4);
					break;

				case 3:
					tgi_bar(4, 75, 159 - 4, 101 - 4);
					break;

				case 4:
					break;
				
				case 5:
					stage = 1;
					delay = 0;
					break;
			}

			if (delay % randomNum == 0) {
				tgi_clear();

				if (randomNum > 13) {
					tune01[11] = '0';
				}
				else {
					tune01[11] = '1';
				}

				if (randomNum < 10) {
					char buf[1];
					tune01[2] = randomNum;
				}
				if (randomNum < 2) {
					tune01[5] = tune01[2];
				}
				else {
					tune01[5] = '6';
				}
			}

			tgi_updatedisplay();
		}
	};
}
