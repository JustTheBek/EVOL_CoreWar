;name Dwarf
;author Unknown
;strategy Classic bomb/skip loop
;assert CORESIZE >= 8000

start   add #4, ptr
        mov bomb, @ptr
        jmp start

ptr     dat #0
bomb    dat #0
