#include <stdlib.h>
#include <string.h>
#include <6502.h>
#include <lynx.h>
#include <tgi.h>

extern char lynxtgi[];
extern char lynxvirus[];
extern char noinfect[];
extern void abcstop();
extern void abcplay(unsigned char channel, char *tune);
extern unsigned char abcactive[4];
