; Evolved warrior
; assert CORESIZE==8000
ORG 0
NOP.F #0, #0
MOV $15, #15
NOP.F #0, #0
SPL $5
SPL $5
JMP $0
MOV $10, $0
JMP #0
ADD @10, $6
NOP.F #0, #0
MOV $0, $10
NOP.F #0, #0
MOV @0, #10
MOV @15, @0
MOV @0, @1
