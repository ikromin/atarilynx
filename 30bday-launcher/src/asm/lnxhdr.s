;******************************************************************************
;Atari Gamer's Atari Lynx 30th Birthday Competition ROM Launcher
;
;Copyright (C) 2019 Igor Kromin
;
;This program is free software: you can redistribute it and/or modify it under
;the terms of the GNU General Public License as published by the Free Software
;Foundation, either version 3 of the License, or (at your option) any later
;version.
;
;This program is distributed in the hope that it will be useful, but WITHOUT
;ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
;FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License along with
;this program.  If not, see <https://www.gnu.org/licenses/>.
;******************************************************************************

; Based on https://bitbucket.org/atarilynx/lynx/src/cd1c78cf3a25b8e9cb22c930d6204fbe8c6bf3c6/tools/cc65/libsrc/lynx/exehdr.s?at=master&fileviewer=file-view-default

	.import		__BANK0BLOCKSIZE__
	.export		__EXEHDRUSR__: absolute = 1

; ------------------------------------------------------------------------
	.segment "EXEHDRUSR"
	.byte	'L','Y','N','X'				              ; magic
	.word	__BANK0BLOCKSIZE__				          ; bank 0 page size
	.word	0                                   ; bank 1 page size
	.word	1					                          ; version number
  .asciiz	"LYNX 30 BIRTHDAY SD LAUNCHER   " ; 32 bytes cart name
	.asciiz	"ATARIGAMER.COM "	                ; 16 bytes manufacturer
	.byte	0					                          ; no rotation
  .byte 0                                   ; aud bits 1=in use for addressing
  .byte 64                                  ; no eeprom, SD card saves supported (to keep power on the MCU for the launcher)
  .byte 0,0,0                               ; spare
