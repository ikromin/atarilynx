# Interrupt Demo

This is a demo that shows how to create a very simple interrupt handler in assembly. This particular interrupt handler fires on the VBL (vertical blank) interrupt.

# Usage

Just watch the interrupt counter go up and up and then roll back over to zero and start again.

# Building

To build run ```make all```. This will produce a file ```timer.o``` that can be run in an emulator.
