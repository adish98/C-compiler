;missing opreand
MAIN: mov  LENGTH
LOOP: jmp L1
;missing opreand
prn 
;missing opreand
bne 
;missing comma
sub @r1 @r5
bne END
;wrong register
add @r1 ,@r10
L1: inc K
bne LOOP
END: stop
;missing apostrophes
STR: .string abcdef"
;illegal characters
LENGTH: .data 6,-9,15,abs
K: .data 22
