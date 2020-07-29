;******************************************************************************
; Atari Gamer's Atari Lynx 30th Birthday Competition ROM Launcher
;
; Copyright (C) 2019 Igor Kromin
;
; This program is free software: you can redistribute it and/or modify it under
; the terms of the GNU General Public License as published by the Free Software
; Foundation, either version 3 of the License, or (at your option) any later
; version.
;
; This program is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
; FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License along with
; this program.  If not, see <https://www.gnu.org/licenses/>.
;******************************************************************************


; Store commonly used runtime variables in the zeropage memory segment
; Amount of memory used (bytes):
;     joystick      3
;     timing       10
;     pointers      8
;     --------    ---
;                  21

.segment "ENGZP" : zeropage


; Variables for joystick state

__engProcessInputs:               .byte 0
.exportzp __engProcessInputs

__engJoyState:                    .byte 0
.exportzp __engJoyState

__engPreviousJoyState:            .byte 0
.exportzp __engPreviousJoyState


; Variables for timing the game loop

__engTimePrevious:                .dword 0
.exportzp __engTimePrevious

__engTimeCurrent:                 .dword 0
.exportzp __engTimeCurrent

__engTimeElapsed:                 .byte 0
.exportzp __engTimeElapsed

__engTimeLag:                     .byte 0
.exportzp __engTimeLag


; Function pointers to state mapped functions

__engFuncProcessInput:            .addr 0
.exportzp __engFuncProcessInput

__engFuncUpdate:                  .addr 0
.exportzp __engFuncUpdate

__engFuncRender:                  .addr 0
.exportzp __engFuncRender