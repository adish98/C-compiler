.entry START
.extern EXTERN
MAIN:   mov @r1, LENGTH
        add @r2, @r3
        sub @r4, @r5
        not @r6
        clr @r7
        lea LENGTH, ARRAY
;wrong register
        inc @r10
        dec @r2
;incorrect type of operand
        jmp 5
;unknown command name
        printf LOOP
        red @r1
        prn -5
        jsr SUBROUTINE
        rts
        stop
;Unnecessary text after string
STRINGLABEL:   .string "Hello, World!"asd
;Illegal characters in data
ARRAY:          .data 1, -2, 3, 4, -5;
LENGTH:         .data 5
NEGNUM:        .data -10
POSNUM:        .data 20
EXTERNNUM:     .data 30
START:  mov @r0, POSNUM
;missing text after label
LOOP:   
        bne END
        jsr SUBROUTINE
        jmp LOOP
SUBROUTINE: add @r3, @r4
        rts
;invlid label name - empty label! (only ':')
:    stop
