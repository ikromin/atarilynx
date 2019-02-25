.interruptor _vbl
.include "lynx.inc"
.export _vblcount
 
_vblcount:
	.byte   $00
 
.proc   _vbl: near
 
.segment "CODE"
	lda INTSET
	and #TIMER2_INTERRUPT
	beq done

	inc _vblcount
 
done:
	clc
	rts
	
.endproc