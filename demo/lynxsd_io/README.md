# Lynx SD I/O Demo

This is a demo that shows how data can be read from and written to the Lynx SD cartridge using its file API.

# Usage

Use the A button to change colours. Use the B button to save the currently selected colour to the save file.

On startup the last saved colour will be restored from the save file.

Note that the ```state.sav``` file must be put on the SD card in the ```saves``` directory.

# Building

To build run ```make all```. This will produce a file ```lynxsdio.o``` that can be run from the Lynx SD cartridge.
