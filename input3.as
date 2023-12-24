.entry START
.extern EXTERN
MAIN:   mov @r1, LENGTH
        add @r2, @r3
        sub @r4, @r5
        not @r6
        clr @r7
        lea LENGTH, ARRAY
        inc @r3
        dec @r2
        jmp LOOP
        bne LOOP
        red @r1
        prn -5
        jsr SUBROUTINE
        rts
        stop
STRINGLABEL:   .string "Hello, World!"
ARRAY:          .data 1, -2, 3, 4, -5
LENGTH:         .data 5
NEGNUM:        .data -10
POSNUM:        .data 20
EXTERNNUM:     .data 30
START:  mov @r0, POSNUM
LOOP:   cmp @r1, @r2
        bne END
        jsr SUBROUTINE
        jmp LOOP
SUBROUTINE: add @r3, @r4
        rts
END:    stop

