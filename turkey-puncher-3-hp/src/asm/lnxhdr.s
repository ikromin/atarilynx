;******************************************************************************
;Turkey Puncher 3 Turbo: Hyper Punching. An April Fools' parody game.
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
	.asciiz	"TURKEY PUNCHER 3 HYPER PUNCHING" ; 32 bytes cart name
	.asciiz	"ATARIGAMER.COM "	                ; 16 bytes manufacturer
	.byte	0					                          ; rotation 1=left
                                            ; rotation 2=right
  .byte   0                                 ; aud bits 1=in use for addressing
  .byte   0                                 ; eeprom
                                            ; eeprom [2:0] -
                                            ; 0 - no eeprom
                                            ; 1 - 93c46 16 bit mode (used in Ttris, SIMIS, Alpine Games, ..., MegaPak I at least)
                                            ; 2        56
                                            ; 3 - 93c66 16 bit mode
                                            ; 4        76
                                            ; 5 - 93c86 16 bit mode
                                            ; (remark: size in bits is 2^(value+9) -- (please recheck!)
                                            ; eeprom [3-5] - reserved - keep it to 0 for further usage
                                            ; eeprom [6] - 0 - real eeprom, 1 - eeprom as a file in /saves/flappy.hi on SD cart
                                            ; eeprom [7] - 0 - 16 bit mode, 1 - 8 bit mode
  .byte   0,0,0                             ; spare
