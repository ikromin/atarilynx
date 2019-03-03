# Lynx SD Menu 2

This is a menu system for loading Atari Lynx ROMs via the Lynx SD Cartridge. Redesigned and updated by [Atari Gamer](https://atarigamer.com).

Features:

 * Atari Lynx ROM loading - LNX, LYX, O, and COM files
 * Game previews via LSD files (with backward compatibility for _PREVIEW directory in the original Lynx SD Menu)
 * Configurable preferences
 * Redesigned user interface
 * Colour theme support
 * Auto-launch ROM on startup option
 * Long ROM name support
 * Support for EEPROM on SD Cart via LNX file header

Original source code from: <http://atariage.com/forums/topic/250637-lynx-multi-card-preorders/>
Lynx SD cartridge is made by [RetroHQ](http://www.retrohq.co.uk).

# Installation
Unzip the ```aglnxmenu2.zip``` file in the top/root level directory of the SD card. This will create the following files and directories:

```
menu/
	alien.pal
	default.pal
	fluoro.pal
	grey.pal
	inverse.pal
	prefs
	homebrew
	lastrom
_preview/
	...(many .lsd files)
menu.bin
```

# ROM List File

By placing a file called ```romlist.txt``` in a directory you can specify long ROM names for each of the 8.3 DOS file names.
An example file is like so...

```
[gateszen.lnx]Gates of Zendocon
[alien.lnx]Alien vs Predator
[apb.lnx]All Points Bulletin
[awegolf.lnx]Awesome Golf
```

Note that the short DOS (8.3) file name is in square brackets, followed by the long ROM name. Maximum length for a ROM name is 50 characters.

The DOS (8.3) names could be in the form ```romnam~1.lnx```. For the ROM list file to work, you have to specify the ROM file name in this format too. On Windows you can find out these short names from the command prompt by using the ```dir /x``` command in the directory where ROM files appear. Alternatively look in the Lynx SD menu file list and wherever there is an up arrow (↑) in the ROM file name, replace it with a tilde (~) to use in the ROM list file.

Example...

```
AUSTRA↑1.O
```

Should have an entry like this in the ```romlist.txt``` file...

```
[AUSTRA~1.O]Australia Day Mini-Demo
```

Any ROMs listed in the ROM list file will be automatically shown in the loader menu, even if those ROMs do not exist on the SD card.

# Custom Colour Schemes

The colour scheme used for the menu interface can be changed by changing the palette specified in the ```menu/default.pal``` file. This is a 32 byte binary file that sets the Lynx palette using BGR colour values. See 'Another approach for setting palette' over at https://atarilynxdeveloper.wordpress.com/2012/04/25/programming-tutorial-part-6colors/ for more information.

A number of sample palettes have been provided in the menu directory, copy any of the ```.pal``` files and rename as ```default.pal``` for changes to take affect.

If no palette file is found, the default colour scheme will be used.

# Change Log

Version 2.0 Atari Gamer

* Whole new UI experience
 * Faster scrolling in the file list (controlled by preference option)
 * Scrollbar showing location of selected file in the overall list
 * Moving back a directory is done with the B button
* Load last ROM feature optimised to start a little faster
  * Toggled by holding down B on boot
* Help screen shown on startup (controlled by preference option)
* Load last ROM can be set to automatic in preferences
* ROM preview image is loaded by pressing the Pause button
* Long names for ROMS (up to 50 characters, via a romlist.txt file)
* Low power mode is not triggered on ROMs that set EEPROM as file in their header
* Support for custom colour schemes using a palette file

Below is the original Change Log from the original sources. This information has now been removed from all source files and will be maintained here instead.

```
Initial version by SainT - Retro HQ

22-05-2016	v1.2	GadgetUK
* Changed interface colours.
* Added Retro HQ logo.
* Added support for left and right directions, flip, and B button to go back.

26-05-2016  v1.3	GadgetUK
* Added support for .LSD files (full screen icon + palette, could be extended to contain publisher info etc).

29-05-2016  v1.4	GadgetUK
* Added support for .LYX and .O files - automatic block sizing based on file size.

29-05-2016  v1.5  GadgetUK
* Removed the facility to show 20 chars of text from the end of the .LSD file - It's too slow.

01-06-2016  v1.6  SainT
* Completed homebrew support.

25-06-2016  v1.7  GadgetUK
* Added .com files into allowed list (same header as other homebrew)
* Also fixed problem with Power Factor and added code to change text colour for loading message.

25-06-2016  v1.8  GadgetUK
* Added delay to homebrew loading - seemed to randomly fail to load.
* Also fixed colour bug related to previews, and scaling bug related to previews.
```

# Building from Source

To build run ```make all```. This will produce a file ```menu.bin``` that can be used on a SD card inside the Lynx SD Atari Lynx cartridge.