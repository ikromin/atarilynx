.interruptor _vbl
.include "lynx.inc"
.export _joystickActionDelay
 
_joystickActionDelay:
	.byte   $00
 
.proc   _vbl: near
 
.segment "CODE"
	lda INTSET
	and #TIMER2_INTERRUPT
	beq done

	lda _joystickActionDelay
	beq done
	dec _joystickActionDelay
 
done:
	clc
	rts

.endproc