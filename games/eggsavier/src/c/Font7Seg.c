/******************************************************************************
 Eggsavier's Cackleberry Rescue

 Copyright (C) 2019 Igor Kromin

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#include <tgi.h>
#include "Font7Seg.h"

/*
 ******************************************************************************
 7-Segment Number display font
 ******************************************************************************
 */

extern char img_num_0[];
extern char img_num_1[];
extern char img_num_2[];
extern char img_num_3[];
extern char img_num_4[];
extern char img_num_5[];
extern char img_num_6[];
extern char img_num_7[];
extern char img_num_8[];
extern char img_num_9[];

static SCB_RENONE seg7FontSprite[] = {
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_0[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_1[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_2[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_3[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_4[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_5[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_6[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_7[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_8[0], 0, 0 },
  { BPP_4 | TYPE_NORMAL, RENONE | PACKED | REUSEPAL, 0x0, 0x0, &img_num_9[0], 0, 0 }
};
SCB_RENONE* sprite = &(seg7FontSprite[0]);

void __fastcall__ draw7SegNumber(u8 x, u8 y, const unsigned int num) {
  if (num == 0) {
    sprite = &(seg7FontSprite[0]);
    sprite->hpos = x;
    sprite->vpos = y;
    tgi_sprite(sprite);
  }
  else {
    int value = num;
    int digit, digits = 0;

    // count number of digits in number
    while (value) {
        value /= 10;
        digits++;
    }

    // draw digits in reverse order from bottom up
    value = num;
    y += (digits - 1) * 10;
    while (value) {
      digit = value % 10;

      sprite = &(seg7FontSprite[digit]);
      sprite->hpos = x;
      sprite->vpos = y;

      tgi_sprite(sprite);

      value /= 10;
      y -= 10;
    }
  }
}