#include <stdlib.h>
#include <6502.h>
#include <lynx.h>
#include <tgi.h>

#define FLICK_FRAMES 20
#define BAR_COLOUR_1 13
#define BAR_COLOUR_2 14

extern char lynxtgi[];

extern char flag[];

extern void abcstop();
extern void abcplay(unsigned char channel, char *tune);
extern unsigned char abcactive[4];
