; The abcmusic for the Lynx is based on the abcmusic notation
; but the implementation is made by Karri Kaksonen

; The Lynx hardware keeps the sound going on without CPU support.
; During music interrupts the CPU will control the volume changes for fadeing
; out the instrument sounds.

; In this version we also have a abcsample player that plays out a digital
; sample at half the HBL rate, close to 4kHz. (framerate * 105 / 2)
; The pcmsound should be in 2's complement 8-bit format 128, 129..255,0,1..127

; You can interrupt a sound playing by starting a new sound with abcplay()

; The numbers work on bytes currently. Setting delays should be capped at 255

; extern void abcstop();
; extern void abcplay(char chan, char *tune);
; extern void abcsound(char chan, char *pcmsound);

  .export _abcstop
  .export _abcplay
  .export _abcval
;  .export _abcsound
  .interruptor _abcmusic_irq

  .import popax
  .importzp _abc_score_ptr0
  .importzp _abc_score_ptr1
  .importzp _abc_score_ptr2
  .importzp _abc_score_ptr3

  .code
endoftune:
  .byte 0

_abcval:
  .byte 0
  .byte 0

; Stop all music
_abcstop:
  lda #$00
  sta $fd50 ; all channels to left+right
  lda #<endoftune
  ldy #>endoftune
  ldx #0
  jsr _abc_set_score
  lda #<endoftune
  ldy #>endoftune
  ldx #1
  jsr _abc_set_score
  lda #<endoftune
  ldy #>endoftune
  ldx #2
  jsr _abc_set_score
  lda #<endoftune
  ldy #>endoftune
  ldx #3
  jmp _abc_set_score

; Read an unsigned integer
; A will contain low byte of nr.
; X is channel, Y is music ptr
abc_read_number:
  stz abc_tmp
  stz abc_tmp+1
abc_read_number_next:
  jsr abc_peek_char
  cmp #$2f ; '/'
  bcc abc_read_number_complete
  cmp #$39 ; '9'
  bcs abc_read_number_complete
  bra abc_read_number_mpy_by_ten_and_add
abc_read_number_complete:
  lda abc_tmp
  rts
abc_read_number_mpy_by_ten_and_add:
  jsr abc_read_char
  and #$0f
  phx		; Save X
  pha
  asl abc_tmp	; multiply by 2
  rol abc_tmp+1
  lda abc_tmp
  ldx abc_tmp+1
  asl abc_tmp	; multiply by 4
  rol abc_tmp+1
  asl abc_tmp
  rol abc_tmp+1 ; multiply by 8
  clc		; add (multiply by 2) = 10
  adc abc_tmp
  sta abc_tmp
  txa
  adc abc_tmp+1
  sta abc_tmp+1
  pla		; add read character
  clc
  adc abc_tmp
  sta abc_tmp
  plx		; Restore X
  lda #0
  adc abc_tmp+1
  sta abc_tmp+1
  bra abc_read_number_next

; Play the music
_abcplay:
  bit abc_init_done
  bne init_done
  jsr abcinit
init_done:
  jsr popax   ; pop tune string
  sta abc_tmp
  stx abc_tmp+1
  jsr popax   ; pop channel
  tax
  lda abc_tmp
  ldy abc_tmp+1
; Activate score on channel X
; A - low address
; Y - high address
_abc_set_score:
  inx
  dex
  bne abc_set_score2
    sta _abc_score_ptr0
    sty _abc_score_ptr0+1
    sty abc_repeat_hoffs
    stz abc_music_ptr
    stz sound_channel_current_len
    rts
abc_set_score2:
  dex
  bne abc_set_score4
    sta _abc_score_ptr1
    sty _abc_score_ptr1+1
    sty abc_repeat_hoffs+1
    stz abc_music_ptr+1
    stz sound_channel_current_len+1
    rts
abc_set_score4:
  dex
  bne abc_set_score6
    sta _abc_score_ptr2
    sty _abc_score_ptr2+1
    sty abc_repeat_hoffs+2
    stz abc_music_ptr+2
    stz sound_channel_current_len+2
    rts
abc_set_score6:
  sta _abc_score_ptr3
  sty _abc_score_ptr3+1
  sty abc_repeat_hoffs+3
  stz abc_music_ptr+3
  stz sound_channel_current_len+3
  rts

; Run-time processing of the envelopes and durations
abcupdate:
  ldx #0
abcupdate_channel_x:
  lda sound_channel_current_len,x
  bne abcupdate_current_len
    ; note has ended, fetch next
    lda abc_music_ptr,x
    tay
    bra parse_abc
abcupdate_current_len:
  lda sound_channel_current_len,x
  ; note is playing
  cmp #255 ; Duration 255 is forever, good for engines
  beq abcupdate_attack
    ; Decrement the duration of the note
    dea
    sta sound_channel_current_len,x
    bne abcupdate_channel_decay
      ; Volume to zero at end of note
      sta sound_channel_current_volume,x
abcupdate_channel_decay:
  ; Calclate how long the note has been playing
  lda abc_note_length,x
  sec
  sbc sound_channel_current_len,x
  cmp abc_instrument_maxlen,x
  bcc abcupdate_attack
    ; Past the H time. Start decaying
    lda sound_channel_current_volume,x
    sec
    sbc abc_instrument_decr,x
    bcs abcupdate_store_decay_volume
      ; silence
      lda #0
abcupdate_store_decay_volume:
    sta sound_channel_current_volume,x
    bra abcupdate_setvolume
abcupdate_attack:
  ; We are in the attack phase
  lda sound_channel_current_volume,x
  cmp abc_channel_volume,x
  bcs abcupdate_setvolume
    ; attack time
    clc
    adc abc_instrument_incr,x
    bcc update_music6
      ; desired volume reached
      lda sound_channel_current_volume,x
update_music6:
    sta sound_channel_current_volume,x
abcupdate_setvolume:
  lda sound_channel_current_volume,x
  phx
  pha
  txa
  clc
  rol
  clc
  rol
  clc
  rol
  tax
  pla
  sta $fd20,x
  plx
  inx
  txa
  cmp #4
  bne abcupdate_channel_x
  rts

; Parse score enough to get next note
; X - channel to use
; Y - abc music pointer
parse_abc:
  jsr abc_read_char
  cmp #$0 ; End of music
  bne parse_abc1
    bra abcupdate_current_len
  lda sound_channel_current_len,x
parse_abc1:
  cmp #$20 ;' ' ignore spaces
  bne parse_abc2
    bra parse_abc
parse_abc2:
  cmp #$7c ;'|'
  bne parse_abc6
    jsr abc_read_char
    cmp #$3a ;':'
    bne parse_abc4
      tya
      sta abc_repeat_offs,x
      phx
      txa
      asl
      tax
      lda _abc_score_ptr0+1,x
      plx
      sta abc_repeat_hoffs,x
      lda #2
      sta abc_repeat_cnt,x
    bra parse_abc5
parse_abc4:
      dey
parse_abc5:
    jmp parse_abc
parse_abc6:
  cmp #$3a ;':'
  bne parse_abc7
    lda abc_repeat_cnt,x
    dea
    sta abc_repeat_cnt,x
    beq parse_abc7
      phy
      lda abc_repeat_hoffs,x
      pha
      txa
      asl
      tax
      pla
      sta _abc_score_ptr0+1,x
      txa
      clc
      ror
      tax
      lda abc_repeat_offs,x
      tay
      pla
      sta abc_repeat_offs,x
      jmp parse_abc
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
  bne parse_abc8b
    jsr abc_read_number
    sta _abcval
    sta abc_instrument_maxlen,x
    jmp parse_abc
parse_abc8b:
  cmp #'K' ; kill sound
  bne parse_abc8c
    jsr abc_read_number
    sta abc_instrument_decr,x
    jmp parse_abc
parse_abc8c:
  cmp #'I' ; incremental flag
  bne parse_abc9
    jsr abc_read_number
    phx
    txa
    clc
    rol
    clc
    rol
    clc
    rol
    tax
    lda abc_tmp
    cmp #0
    bne parse_abc8d
      lda $fd25,x
      and #$df
    bra parse_abc8e
parse_abc8d:
      lda $fd25,x
      ora #$20
parse_abc8e:
    ora #$18
    sta $fd25,x
    plx
    jmp parse_abc
parse_abc9:
  cmp #$54 ;'T' tempo
  bne parse_abc9b
    jsr abc_read_number
    sta abc_tempo,x
    jmp parse_abc
parse_abc9b:
  cmp #$4f ;'O' octave
  bne parse_abc9c
    jsr abc_read_number
    phx
    txa
    clc
    rol
    clc
    rol
    clc
    rol
    tax
    lda $fd25,x
    and #$f8
    ora #$18
    ora abc_tmp
    sta $fd25,x
    plx
    jmp parse_abc
parse_abc9c:
  cmp #'X'; XOR taps
  bne parse_abc10
    jsr abc_read_number
    phx
    txa ; modify X to point to sound channel
    clc
    rol
    clc
    rol
    clc
    rol
    tax
    lda abc_tmp
    ; The two topmost bits are shifted one place right
    bpl parse_abc9d
      ora #$40
    bra parse_abc9e
parse_abc9d:
      and #$bf
parse_abc9e:
    dec abc_tmp+1
    beq parse_abc9f
      and #$7f
    bra parse_abc9g
parse_abc9f:
      ora #$80
parse_abc9g:
    sta $fd21,x
    ; Bit $40 is put in a different register
    lda abc_tmp
    and #$40
    bne parse_abc9h
      lda $fd25,x
      and #$7f
    bra parse_abc9i
parse_abc9h:
      lda $fd25,x
      ora #$80
parse_abc9i:
    ora #$18
    sta $fd25,x
    lda #0
    sta $fd23,x
    plx
    jmp parse_abc
parse_abc10:
  cmp #$7a ;'z'
  bne parse_abc11
    lda #0
    bra set_music_ptr
parse_abc11:
  ; Find out the pitch of the note
  stz cur_note
  inc cur_note
  cmp #$3d ;'='
  bne parse_abc12
    inc cur_note
    jsr abc_read_char
parse_abc12:
  cmp #$7e ;'~'
  bne parse_abc13
    dec cur_note
    jsr abc_read_char
parse_abc13:
  sec
  sbc #$41 ;'A'
  cmp #8 ;'H'-'A'
  bcs parse_abc14 ;_IFLO
    clc
    asl
    clc
    adc cur_note
    sta cur_note
  bra parse_abc15
parse_abc14:
    sec
    sbc #$20 ;'a'-'A' + 15
    clc
    asl
    clc
    adc cur_note
    clc
    adc #15
    sta cur_note
parse_abc15:
  lda cur_note
  phy
  tay
  lda _delays,y
  phx
  pha
  txa
  clc
  rol
  clc
  rol
  clc
  rol
  tax
  pla
  sta $fd24,x
  plx
  ply
  ; Find out the volume of the note
  lda abc_instrument_incr,x
  bne start_with_silence
  lda abc_channel_volume,x
  bra set_music_ptr
start_with_silence:
  lda #0
set_music_ptr:
  sta sound_channel_current_volume,x
  ; Find out the duration of the note
  jsr abc_read_number
  cmp #4 ; "4"
  bne parse_abc16
    lda abc_tempo,x
    clc
    rol
    clc
    rol
  bra parse_abc19
parse_abc16:
    cmp #3 ; "3"
    bne parse_abc17
      lda abc_tempo,x
      clc
      rol
      clc
      adc abc_tempo,x
    bra parse_abc19
parse_abc17:
      cmp #2 ; "2"
      bne parse_abc18
        lda abc_tempo,x
        clc
        rol
      bra parse_abc19
parse_abc18:
        lda abc_tempo,x
parse_abc19:
  sta sound_channel_current_len,x
  sta abc_note_length,x
  tya
  sta abc_music_ptr,x
  ;lda abc_instrument_maxlen,x
  ;cmp sound_channel_current_len,x
  ;bcs parse_small
  ;sta sound_channel_current_len,x
parse_small:
  jmp abcupdate_channel_x

; Peek a character from the score.
; X - channel
; Y - score offset
abc_peek_char:
  txa
  ina
  dea
  bne abc_peek_char_not_chan0
    ; Channel 0
    lda (_abc_score_ptr0),y
    bra abc_peek_char_done
abc_peek_char_not_chan0:
  dea
  bne abc_peek_char_not_chan1
    ; Channel 1
    lda (_abc_score_ptr1),y
    bra abc_peek_char_done
abc_peek_char_not_chan1:
  dea
  bne abc_peek_char_not_chan2
    ; Channel 2
    lda (_abc_score_ptr2),y
    bra abc_peek_char_done
abc_peek_char_not_chan2:
  ; Channel 3
  lda (_abc_score_ptr3),y
abc_peek_char_done:
  rts

; Read a character from the score. Advance ptr if it is not 0
; This routine will also increment the music pointer over page boundaries
; X - channel
; Y - score offset
abc_read_char:
  txa
  ina
  dea
  bne abc_read_char_not_chan0
    ; Channel 0
    lda (_abc_score_ptr0),y
    beq abc_read_char_done
      iny
      bne abc_read_char_done
        inc _abc_score_ptr0+1
    bra abc_read_char_done
abc_read_char_not_chan0:
  dea
  bne abc_read_char_not_chan1
    ; Channel 1
    lda (_abc_score_ptr1),y
    beq abc_read_char_done
      iny
      bne abc_read_char_done
        inc _abc_score_ptr1+1
    bra abc_read_char_done
abc_read_char_not_chan1:
  dea
  bne abc_read_char_not_chan2
    ; Channel 2
    lda (_abc_score_ptr2),y
    beq abc_read_char_done
      iny
      bne abc_read_char_done
        inc _abc_score_ptr2+1
    bra abc_read_char_done
abc_read_char_not_chan2:
  ; Channel 3
  lda (_abc_score_ptr3),y
  beq abc_read_char_done
    iny
    bne abc_read_char_done
      inc _abc_score_ptr3+1
abc_read_char_done:
  rts

; Instrument setup. C-interface
_abcinstrument:
  ; Lynx-specific abc-command to set up the instrument
  jsr popax   ; channel
  sta instrumenttmp+1
  clc
  rol
  clc
  rol
  clc
  rol
  sta instrumenttmp
  jsr popax ; shift 012345 10 11
  ldx instrumenttmp
  sta $fd21,x
  jsr popax ; start value not needed
  ldx instrumenttmp+1
  sta abc_note_length,x
  ldx instrumenttmp
  ; sta $fd23,x
  jsr popax ; backup not needed
  ldx instrumenttmp+1
  sta abc_channel_volume,x
  ldx instrumenttmp
  ; sta $fd24,x
  jsr popax ; flags b012 clock b5 integr b7 fb7
  ldx instrumenttmp
  sta $fd25,x
  jsr popax ; attack
  ldx instrumenttmp+1
  sta abc_instrument_incr,x
  jsr popax ; hold
  ldx instrumenttmp+1
  sta abc_instrument_maxlen,x
  jsr popax ; decay
  ldx instrumenttmp+1
  sta abc_instrument_decr,x
  rts

	.data
instrumenttmp: .res 2
abc_music_ptr:   .byte 0,0,0,0 ; Score pointer
abc_repeat_offs: .byte 0,0,0,0
abc_repeat_hoffs: .byte 0,0,0,0
abc_repeat_cnt:  .byte 0,0,0,0
abc_tempo: .byte 6,6,6,6 ; Total length including tail
abc_note_length: .byte 6,6,6,6 ; Total length including tail
abc_channel_volume: .byte 127,127,127,127 ; Desired max volume of the channel
abc_instrument_incr:   .byte 4,4,4,4 ; Step for incrementing
abc_instrument_maxlen: .byte 4,4,4,4 ; Step to keep note steady
abc_instrument_decr:   .byte 4,4,4,4 ; Step for decrementing
sound_channel_current_volume: .byte 127,127,127,127
sound_channel_current_len:   .byte 4,4,4,4
cur_note:    .byte 0
abc_init_done: .byte 0
abc_tmp: .res 2

	.rodata
; This table is used to cover the delays needed for 2 octaves
_delays: .byte 161 ; Ab
        .byte 152 ; A
        .byte 143 ; A# Bb
        .byte 135 ; B
        .byte 128 ;
        .byte 255 ; C
        .byte 241 ; C# Db
        .byte 227 ; D
        .byte 214 ; D# Eb
        .byte 202 ; E
        .byte 191 ;
        .byte 191 ; F
        .byte 180 ; F# Gb
        .byte 170 ; G
        .byte 161 ; G#
_delays2:
        .byte 80  ; ab
        .byte 76  ; a
        .byte 72  ; a# bb
        .byte 68  ; b
        .byte 128 ;
        .byte 128 ; c
        .byte 120 ; c# db
        .byte 114 ; d
        .byte 107 ; d# eb
        .byte 101 ; e
        .byte 96  ;
        .byte 96  ; f
        .byte 90  ; f# gb
        .byte 85  ; g
        .byte 80  ; g#

; Last, but no least - tie update mechanism to interrupts

TIMER0_INTERRUPT = $01
TIMER2_INTERRUPT = $04
TIMER7_INTERRUPT = $80
INTSET      = $FD81

	.code

_31250Hz	.set	%101

abcinit:
	rts
	php
	sei
        lda #1
        sta abc_init_done
	lda	#%10011000|_31250Hz
	sta	$FD1D
	lda	#129
	sta	$FD1C	; set up a 240Hz IRQ
	plp
	rts

_abcmusic_irq:	lda	INTSET
	and	#TIMER2_INTERRUPT
	beq	@goon
	jsr	abcupdate
@goon:
	and	#TIMER0_INTERRUPT
	beq	@goon2
	;jsr	_update_music
@goon2:
	clc
	rts

