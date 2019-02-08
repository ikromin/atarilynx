;
; The abcmusic for the Lynx is based on the abcmusic notation
; but the implementation is made by Karri Kaksonen, 02.02.2019
;
; The macro package is by Bjoern Spruck. Taken from Chipper.
;
; The Lynx hardware keeps the sound going on without CPU support.
; During music interrupts the CPU will control the volume changes for fadeing
; out the instrument sounds.
;
; You can interrupt a sound playing by starting a new sound with abcplay()
;
; The numbers work on bytes currently. Setting delays should be capped at 255
;

;----------------------------------------------------------------------------
;
; C entrypoints
;
; You need to call abcstop() at init time before using abcmusic
;
;
; extern void abcplay(char chan, char *tune);
; extern void abcstop();
; extern unsigned char abcactive[4];
;
; The tune supports the following elements:
; Notes CDEFGAB
; Octave higher cdefgab
; Sharp notes ^C ^D...
; Flat notes _D _E...
; You can also make a note one octave lower by a comma C, D,
; Or one octave higher with an apostrophe c' d'
; Break in the music is z z2 z3
; Longer notes C2 C3 C4
;
; For direct sound control there is also a few exposer internals
; extern void __fastcall__ abcoctave(unsigned char chan, unsigned char val);
; extern void __fastcall__ abcpitch(unsigned char chan, unsigned char val);
; extern void __fastcall__ abctaps(unsigned char chan, unsigned int val);
; extern void __fastcall__ abcintegrate(unsigned char chan, unsigned char val);
; extern void __fastcall__ abcvolume(unsigned char chan, unsigned char val);
; 

	.export _abcstop
	.export _abcplay
	.export _abcactive
	.export _abcoctave
	.export _abcpitch
	.export _abctaps
	.export _abcintegrate
	.export _abcvolume
	.interruptor _abcmusic_irq

	.import popax
	.import popa
	.importzp _abc_score_ptr0
	.importzp _abc_score_ptr1
	.importzp _abc_score_ptr2
	.importzp _abc_score_ptr3


;----------------------------------------------------------------------------
;
; Variables
;
	.segment "DATA"
init_done:			; Init done flag
	.byte 0
endoftune:			; Dummy "empty" tune used for stopping play
	.byte 0
_abcactive:			; Read only active flags for 4 channels
	.byte 0,0,0,0
abc_music_ptr:			; Score pointer
	.byte 0,0,0,0
abc_repeat_offs:		; Something to do with repeat of sections
	.byte 0,0,0,0
abc_repeat_cnt:			; Something to do with repeat of sections again
	.byte 0,0,0,0
abc_tempo:			; Store tempo of every score 'T'
	.byte 6,6,6,6
abc_channel_volume:		; Store volume if every score 'V'
	.byte 127,127,127,127
abc_instrument_incr:		; Store attack increment step 'R'
	.byte 4,4,4,4
abc_instrument_maxlen:		; Store how long to hold the note 'H'
	.byte 4,4,4,4
abc_instrument_decr:		; Store decay decrement step 'K'
	.byte 4,4,4,4
abc_note_length:		; Total note length from start of attack until end of decay
	.byte 6,6,6,6
sound_channel_current_volume:	; Current volume of channel
	.byte 127,127,127,127
note_playing:			; Current length of note playing
	.byte 4,4,4,4
cur_note:			; Temporary variable
	.byte 0
cur_octave:			; Temporary variable
	.byte 0

	.segment "RODATA"
; This table is used to cover the delays needed for 1 octave
_delays:
	.byte 255 ; C
	.byte 240 ;^C
	.byte 227 ; D
	.byte 214 ;^D
	.byte 202 ; E
	.byte 191 ;^E
	.byte 191 ; F
	.byte 180 ;^F
	.byte 170 ; G
	.byte 161 ;^G
	.byte 152 ; A
	.byte 143 ;^A
	.byte 135 ; B
	.byte 127 ;^B

	.segment "BSS"
instrumenttmp:
	.res 2
abc_number:
	.res 2
abc_tunestr:
	.res 2
abc_oct:
	.res 4
abc_tmp2:
	.res 1

	.segment "CODE" 
;----------------------------------------------------------------------------
; Macros
;

if_count	.set	0
nest_count	.set	0

.macro	_IFNE
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	beq		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro	_IFEQ
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bne		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro	_IFMI
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bpl		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro	_IFPL
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bmi		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro	_IFGE
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bcc		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFCS
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bcc		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFCC
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bcs		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _ELSE
	bra	.ident (.sprintf ("endif%04d", .ident (.sprintf ("push%04d", nest_count))))
	.ident  (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
.endmacro

.macro  _ENDIF
	.if .not .defined( .ident (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))))
		.ident  (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
	.endif
	.ident  (.sprintf ("endif%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
	nest_count	.set	nest_count -1
.endmacro

;----------------------------------------------------------------------------
;
; Play the music
; extern void abcplay(char chan, char *tune);
;
_abcplay:
	lda init_done
	bne @L1
		jsr _abcstop
		inc init_done
@L1:
	jsr popax   ; pop tune
	sta abc_tunestr
	stx abc_tunestr+1
	jsr popa    ; pop channel
	tax
	lda abc_tunestr
	ldy abc_tunestr+1

; Activate score on channel X
; X - channel
; A - low address
; Y - high address
abc_set_score:
	inx
	dex
	_IFEQ
		sta _abc_score_ptr0
		sty _abc_score_ptr0+1
		lda #0
		sta abc_music_ptr
		sta note_playing
		jsr volume
		ina
		sta _abcactive
		rts
	_ENDIF
	dex
	_IFEQ
		sta _abc_score_ptr1
		sty _abc_score_ptr1+1
		lda #0
		sta abc_music_ptr+1
		sta note_playing+1
		jsr volume
		ina
		sta _abcactive+1
		rts
	_ENDIF
	dex
	_IFEQ
		sta _abc_score_ptr2
		sty _abc_score_ptr2+1
		lda #0
		sta abc_music_ptr+2
		sta note_playing+2
		jsr volume
		ina
		sta _abcactive+2
		rts
	_ENDIF
	sta _abc_score_ptr3
	sty _abc_score_ptr3+1
	lda #0
	sta abc_music_ptr+3
	sta note_playing+3
	jsr volume
	ina
	sta _abcactive+3
	rts

;----------------------------------------------------------------------------
;
; Stop all music
;
; extern void abcstop();
;
_abcstop:
	lda #$00
	sta _abcactive
	sta _abcactive+1
	sta _abcactive+2
	sta _abcactive+3
	sta $fd50		; all channels to left+right
	lda #<endoftune
	ldy #>endoftune
	ldx #0
	jsr abc_set_score
	lda #<endoftune
	ldy #>endoftune
	ldx #1
	jsr abc_set_score
	lda #<endoftune
	ldy #>endoftune
	ldx #2
	jsr abc_set_score
	lda #<endoftune
	ldy #>endoftune
	ldx #3
	jmp abc_set_score

;----------------------------------------------------------------------------
;
; Read an unsigned integer
; A will contain lower byte result
; X is channel, Y is music ptr
;
abc_read_number:
	stz abc_number
	stz abc_number+1
@L1:
	jsr abc_peek_char
	cmp #$2f ; '/'
	bcc @L2
	cmp #$39 ; '9'
	bcs @L2
	jsr abc_read_char
	and #$0f		; A contains the number we want to add
	phx
	pha
	asl abc_number	; multiply by 2
	rol abc_number+1
	lda abc_number
	ldx abc_number+1
	asl abc_number
	rol abc_number+1
	asl abc_number	; multiply by 8
	rol abc_number+1
	clc
	adc abc_number
	sta abc_number	; multiplied by 10
	txa
	adc abc_number+1
	sta abc_number+1
	pla
	clc
	adc abc_number
	sta abc_number	; abc_number = 10*abc_number + A
	plx
	lda #0
	adc abc_number+1
	sta abc_number+1
	bra @L1
@L2:
	lda abc_number
	rts

;----------------------------------------------------------------------------
;
; Start repeat section
;
; X - channel
; Y - tune pointer
startrepeat:
	jsr abc_read_number
	_IFEQ
		lda #255
	_ENDIF
	sta abc_repeat_cnt,x		; Store at least 1 as repeatcnt
	tya				; Store tune pointer
	sta abc_repeat_offs,x
	rts

;----------------------------------------------------------------------------
;
; End repeat section
;
endrepeat:
	lda abc_repeat_cnt,x		; End of repeat section
	cmp #255
	_IFNE
		dea
		sta abc_repeat_cnt,x
	_ENDIF
	cmp #0
	_IFNE
		lda abc_repeat_offs,x
		tay
	_ENDIF
	rts

;----------------------------------------------------------------------------
;
; Integrate flag
;
integrate:
	phx
	pha
	txa
	asl
	asl
	asl
	tax
	pla
	cmp #0
	_IFEQ
		lda $fd25,x	; Square wave
		and #$df
	_ELSE
		lda $fd25,x	; Triangle wave
		ora #$20
	_ENDIF
	ora #$18
	sta $fd25,x
	plx
	rts

;----------------------------------------------------------------------------
;
; Choose square wave or integrate
; extern void __fastcall__ abcintegrate(unsigned char chan, unsigned char val);
;
_abcintegrate:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp integrate

;----------------------------------------------------------------------------
;
; Process polynomic sound generator taps
;
taps:
	phy
	tay
	phx
	txa ; modify X to point to sound channel
	asl
	asl
	asl
	tax
	tya
	sta $fd21,x
	plx
	ply
	rts

taps40:
	phy
	tay
	phx
	txa ; modify X to point to sound channel
	asl
	asl
	asl
	tax
	; Bit $40 is put in a different register
	tya
	and #$40
	_IFEQ
		lda $fd25,x
		and #$7f
	_ELSE
		lda $fd25,x
		ora #$80
	_ENDIF
	ora #$18
	sta $fd25,x
	lda #0
	sta $fd23,x
	plx
	ply
	rts

;----------------------------------------------------------------------------
;
; Choose polynomic feedback taps
; extern void __fastcall__ abctaps(unsigned char chan, unsigned int val);
;
_abctaps:
	pha	; Push original abc_number low byte
	; The two topmost bits are shifted one place right
	bpl @L1
		ora #$40
		bra @L2
@L1:
		and #$bf
@L2:
	dex
	bne @L3
		ora #$80
		bra @L4
@L3:
		and #$7f
@L4:
	pha		; Get channel number to X
	jsr popa
	tax
	pla
	jsr taps	; Set most taps bits
	pla		; Get bit 40 that goes to different register
	lda #0
	jsr taps40
	rts

;----------------------------------------------------------------------------
;
; Find out the pitch of the note
;
; The real low notes are C,D,E,F,G,A,B,
; The lower notes are CDEFGAB
; The higher notes are cdefgab
; The real high notes are c'd'e'f'g'a'b'
; Sharps are ^C
; Flats are _D
;
pitch:
	stz cur_note
	inc cur_note
	stz cur_octave
	inc cur_octave
	cmp #$5E ;'^'
	bne @L1
		inc cur_note		; Increment second by '^'
		jsr abc_read_char
	bra @L2
@L1:
	cmp #$5F ;'_'
	bne @L2
		dec cur_note		; Decrement second by '_'
		jsr abc_read_char
@L2:
	sec
	sbc #$41 ;'A'
	cmp #8 ;'H'-'A'
	bcc @L3 ;_IFHI
	sec
	sbc #$20 ;'a'-'A'
	cmp #8 ;'H'-'A'
	bcs @L6 ;_IFLO
	  dec cur_octave
@L3:
	  clc
	  asl
	  clc
	  adc cur_note			; . A . B . C . D . E . F . G . (0..14)
	  cmp #5
	  bcs @L4
	    clc
	    adc #14
@L4:
	  sec
	  sbc #5
	  sta cur_note			; C . D . E . F . G . A . B . (0..14)
	jsr abc_peek_char
	cmp #','
	bne @L5
	  jsr abc_read_char
	  inc cur_octave
	  rts
@L5:
	cmp #$27 ;'
	bne @L6
	  jsr abc_read_char
	  dec cur_octave
@L6:
	rts

;----------------------------------------------------------------------------
;
; Set the BACKUP register on the hw to start running out the pitch
; X - channel to use
; A - desired backup value
;
backup:
	phx
	pha
	txa
	asl
	asl
	asl
	tax
	pla
	sta $fd24,x
	plx
	rts

;----------------------------------------------------------------------------
;
; Choose pitch of the current sound
; extern void __fastcall__ abcpitch(unsigned char chan, unsigned char val);
;
_abcpitch:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp backup

;----------------------------------------------------------------------------
;
; Set the current octave
; X - channel to use
; A - desired octave
;
setoctave:
	phy
	tay
	phx
	txa
	asl
	asl
	asl
	tax
	lda $fd25,x
	and #$f8
	ora #$18
	iny
@L1:
	dey
	beq @L2
	ina
	bra @L1
@L2:
	sta $fd25,x
	plx
	ply
	rts

;----------------------------------------------------------------------------
;
; Choose octave 0..6
; extern void __fastcall__ abcoctave(unsigned char chan, unsigned char val);
;
_abcoctave:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp setoctave

; Run time envelope processing for a note
; X is the channel
; return A, 0 note ended
envelope:
  lda note_playing,x
  bne @L0
  rts					; Note has ended
@L0:
  cmp #255				; Duration is forever
  beq @L3
    dea					; Decrement the duration of the note
    sta note_playing,x
    bne @L1
      sta sound_channel_current_volume,x ; Turn off volume at end of note
      bra @L5
@L1:
  lda abc_note_length,x
  sec
  sbc note_playing,x
  cmp abc_instrument_maxlen,x
  bcc @L3
    lda sound_channel_current_volume,x	; Past the H time. Start decaying
    sec
    sbc abc_instrument_decr,x
    bcs @L2
      ; silence
      lda #0
@L2:
    sta sound_channel_current_volume,x	; Set current decayed volume
    bra @L5
@L3:
  lda sound_channel_current_volume,x
  cmp abc_channel_volume,x
  bcs @L5
    clc					; We are in the attack phase
    adc abc_instrument_incr,x
    bcc @L4
      lda sound_channel_current_volume,x ; desired volume reached
@L4:
    sta sound_channel_current_volume,x
@L5:
  lda sound_channel_current_volume,x
volume:
	phx
	pha
	txa
	asl
	asl
	asl
	tax
	pla
	sta $fd20,x
	plx
	lda #1
	rts

;----------------------------------------------------------------------------
;
; Choose pitch of the current sound
; extern void __fastcall__ abcvolume(unsigned char chan, unsigned char val);
;
_abcvolume:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp volume

; Find out the duration of the note
duration:
  jsr abc_read_number
  bne @L1
  lda #1
@L1:
  phy
  tay
  lda abc_tempo,x
  sta abc_tmp2
  lda #0
@L2:
  clc
  adc abc_tmp2
  dey
  bne @L2
  ply
  sta note_playing,x
  sta abc_note_length,x
  rts

; Last, but no least - tie update mechanism to interrupts

VBL_INTERRUPT = $04
INTSET      = $FD81

_abcmusic_irq:	lda	INTSET
	and	#VBL_INTERRUPT
	beq	@exit
	jsr	abcupdate
@exit:
	clc
	rts

; Run-time processing of the envelopes and durations
abcupdate:
	lda init_done
	bne @L0
		jsr _abcstop
@L0:
	ldx #0
@L1:
	lda _abcactive,x
	beq @L3
		lda note_playing,x
		bne @L2
			; note has ended, fetch next
			lda abc_music_ptr,x
			tay
			jsr parse_abc
		bra @L1
@L2:
		jsr envelope
@L3:
	inx
	txa
	cmp #4
	bne @L1
	rts

; Parse score enough to get next note
; X - channel to use
; Y - abc music pointer
parse_abc:
	jsr abc_read_char
	cmp #$0
	bne @L1
		sta _abcactive,x	; End of music
		rts
@L1:
	cmp #$20			;' ' ignore spaces
	beq parse_abc
	cmp #$7c			;'|'
	bne @L2
		jsr abc_peek_char
		cmp #$3a		;':'
		bne parse_abc
		jsr abc_read_char
		jsr startrepeat
		bra parse_abc
@L2:
	cmp #$3a			;':'
	bne @L3
		jsr endrepeat
		bra parse_abc
@L3:
parse_abc7:
parse_abc8u:
  cmp #'V' ; volume
  bne parse_abc8
    jsr abc_read_number
    sta abc_channel_volume,x
    jmp parse_abc
parse_abc8:
  cmp #'R' ; ramp up
  bne parse_abc8a
    jsr abc_read_number
    sta abc_instrument_incr,x
    jmp parse_abc
parse_abc8a:
  cmp #'H' ; Hold
  bne @L6
    jsr abc_read_number
    sta abc_instrument_maxlen,x
    jmp parse_abc
@L6:
  cmp #'K' ; kill sound
  bne @L7
    jsr abc_read_number
    sta abc_instrument_decr,x
    jmp parse_abc
@L7:
  cmp #'I' 				; integrate flag
  bne @L8
    jsr abc_read_number
    jsr integrate
    jmp parse_abc
@L8:
  cmp #$54 ;'T' tempo
  bne @L9
    jsr abc_read_number
    sta abc_tempo,x
    jmp parse_abc
@L9:
  cmp #$4f ;'O' octave
  bne @L10
    jsr abc_read_number
    sta abc_oct,x
    jmp parse_abc
@L10:
  cmp #'X'; XOR taps
  bne @L15
	jsr abc_read_number
	; The two topmost bits are shifted one place right
	bpl @L11
		ora #$40
		bra @L12
@L11:
		and #$bf
@L12:
	dec abc_number+1
	bne @L13
		ora #$80
		bra @L14
@L13:
		and #$7f
@L14:
	jsr taps
	jsr abc_read_number
	jsr taps40
	jmp parse_abc
@L15:
  cmp #$7a ;'z'
  bne parse_abc11
    lda #0
    bra set_music_ptr
parse_abc11:
    jsr pitch
    lda cur_note
	phy
	tay
	lda _delays,y
	ply
    jsr backup
	lda abc_oct,x			; Get the octave of the instrument
	clc
	adc cur_octave			; Add the octave of the cur_note
	sta cur_octave
    jsr setoctave
  ; Find out the volume of the note
  lda abc_instrument_incr,x
  bne start_with_silence
  lda abc_channel_volume,x
  bra set_music_ptr
start_with_silence:
  lda #0
set_music_ptr:
  sta sound_channel_current_volume,x
  jsr duration
  tya
  sta abc_music_ptr,x
  ;lda abc_instrument_maxlen,x
  ;cmp note_playing,x
  ;bcs parse_small
  ;sta note_playing,x
parse_small:
  rts

; Peek a character from the score.
; X - channel
; Y - score offset
abc_peek_char:
	txa
	ina
	dea
	bne @L1
		lda (_abc_score_ptr0),y
		bra @L4
@L1:
	dea
	bne @L2
		lda (_abc_score_ptr1),y
		bra @L4
@L2:
	dea
	bne @L3
		lda (_abc_score_ptr2),y
		bra @L4
@L3:
	lda (_abc_score_ptr3),y
@L4:
	rts

; Read a character from the score. Advance ptr if it is not 0
; X - channel
; Y - score offset
abc_read_char:
	jsr abc_peek_char
	_IFNE
		iny
	_ENDIF
	rts

