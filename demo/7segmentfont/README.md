# 7-Segment Font Demo

This is a demo that demonstrates how a 7-segment display font can be created and used on Atari Lynx.

# Usage

The following functions are exported by the 7-segment font code:

void __fastcall__ set7SegColor(u8 color);
void __fastcall__ draw7SegNumber(u8 x, u8 y, u8 wx, u8 wy, const unsigned int num);

Example:

```
set7SegColor(4);
draw7SegNumber(3, 40, 7, 0, 123);
```

# Building

To build run ```make all```. This will produce a file ```7segment.lnx``` that can be run in an emulator.
