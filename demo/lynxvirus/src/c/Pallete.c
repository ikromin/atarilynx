#include "Pallete.h"

int palette[] =  {
  
  0x0000, 	0x0f0f, 	0x0fff, 	0x0324, 
	0x0337, 	0x021a, 	0x0319, 	0x0526, 
	0x0556, 	0x0624, 	0x062e, 	0x083b, 
	0x087a, 	0x0b56, 	0x0b6f, 	0x0dbe
};

/**
 * Pallete setting code from https://atarilynxdeveloper.wordpress.com/2012/04/25/programming-tutorial-part-6colors/
 */
void setpalette(const int* palette)
{
  char index;
  for (index = 0; index < 0x10; index++)
  {
    POKE(0xFDA0 + index, palette[index] >> 8);
    POKE(0xFDB0 + index, palette[index] & 0xFF);
  }
}