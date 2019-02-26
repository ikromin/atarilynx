# Lynx SD Menu

This is a menu system for loading Atari Lynx ROMs via the Lynx SD Cartridge.
Original source code from: http://atariage.com/forums/topic/250637-lynx-multi-card-preorders/
Lynx SD cartridge is made by (RetroHQ)[http://www.retrohq.co.uk]

# Building

To build run ```make all```. This will produce a file ```menu.bin``` that can be used on a SD card inside the Lynx SD Atari Lynx cartridge.

# ROM List File

By placing a file called ```romlist.txt``` in a directory you can specify long ROM names for each of the 8.3 DOS file names.
An example file is like so...

```
gateszen.lnx|Gates of Zendocon
alien.lnx|Alien vs Predator
apb.lnx|All Points Bulletin
awegolf.lnx|Awesome Golf
```

Note that there is a pipe separator character between the short DOS (8.3) file name and the long ROM name. Maximum length for a ROM name is 50 characters.

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