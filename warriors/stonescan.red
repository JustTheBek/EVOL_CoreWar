;name StoneScan
;author Example
;strategy Scanner + stone bomber (non-replicating, non-imp)
;assert CORESIZE >= 8000

        ORG     start

step    EQU     3039            ; large prime step to avoid patterns
bomb    DAT     #0, #0

start   ADD     #step, scan     ; move scan pointer
scan    SEQ     bomb, @scan     ; check if memory changed
        JMP     start           ; nothing found → keep scanning

        MOV     bomb, @scan     ; found something → bomb it
        JMP     start

