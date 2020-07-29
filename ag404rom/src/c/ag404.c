#include <stdlib.h>
#include <6502.h>
#include <lynx.h>
#include <peekpoke.h>
#include <tgi.h>

extern char lynxtgi[];
extern char img404[];

SCB_REHV_PAL iconsprite =  {
	BPP_4 | TYPE_BACKGROUND,
	REHV,
	0,
	0,
	&img404[0],
    0, // x
	0, // y
	0x0100, // x scale
	0x0100, // y scale
	{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }  
};

int palette[] =  {
  	0x0000, 	0x000a, 	0x070f, 	0x0999, 
	0x0fff, 	0x0000, 	0x0000, 	0x0000, 
	0x0000, 	0x0000, 	0x0000, 	0x0000, 
	0x0000, 	0x0000, 	0x0000, 	0x0000
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
	tgi_setframerate(60);

	tgi_init();
	CLI();
	while (tgi_busy()) {};

	setpalette(palette);
}

void main(void) 
{	
	initialize();
	
	while (tgi_busy()) {}
	tgi_sprite(&iconsprite);
	tgi_updatedisplay();
}
