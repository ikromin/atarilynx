#include "Pallete.h"

int palette[] =  {
  0x0fff, 0x011f, 0x0692, 0x0946, 0x0404, 0x0818,
  0x0666, 0x0888, 0x0616, 0x0b7f, 0x066e, 0x0b3b, 
  0x0513, 0x0f0f, 0x0f00, 0x0000
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