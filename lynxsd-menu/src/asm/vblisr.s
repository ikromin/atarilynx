.interruptor _vbl
.include "lynx.inc"
.export _inputDelay
 
_inputDelay:
	.byte   $00

.proc   _vbl: near
 
.segment "CODE"
	lda INTSET
	and #TIMER2_INTERRUPT
	beq @done

	lda _inputDelay
	beq @done
	dec _inputDelay

@done:
	clc
	rts

.endproc