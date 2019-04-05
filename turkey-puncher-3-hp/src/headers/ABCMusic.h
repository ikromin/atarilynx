#ifndef __LYNX_ABCMUSIC__
#define __LYNX_ABCMUSIC__

extern void abcstop();
extern void abcplay(unsigned char channel, char *tune);
extern unsigned char abcactive[4];

#endif // __LYNX_ABCMUSIC__