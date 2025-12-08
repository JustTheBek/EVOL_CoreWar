;name Imp
;author Unknown (classic)
;strategy Simple replicator / mover
;assert CORESIZE >= 8000

imp     mov 0, 1   ; copy the instruction at current location to next
        jmp imp    ; jump to the next instruction
