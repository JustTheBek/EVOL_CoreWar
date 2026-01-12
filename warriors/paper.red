;name Paper
;author Example
;assert CORESIZE >= 8000

start   mov 0, 1                ; Copy the instruction at the current memory location (0) to the next memory cell (+1).
                                ; This is the main replication step: it spreads itself forward in memory.

        add  #1, 2              ; Add the literal value 1 (#1) to the value at the address 2 cells ahead.
                                ; This slightly modifies memory as it spreads, which can help it survive simple Imp-style attacks.
                                ; It's optional for pure replication, but here it adds a tiny “twist” to the spreading pattern.

        jmp start               ; Jump back to the label "start" to repeat the process.
                                ; This creates an infinite loop: the warrior keeps copying itself and modifying memory endlessly.
