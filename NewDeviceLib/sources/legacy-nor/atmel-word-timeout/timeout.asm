; Authored timeout return, appended at 51B4. Experimental, never standalone.
; P2=F0 and R0 retain the selected FPGA access. DPTR becomes C800 host port.
; P3.2 high = host port busy. Reply ABI: 00, observed low byte, observed high byte.
; Timer0 is stopped before returning; healthy program path is untouched.
        org     051B4h
        clr     TR0
        clr     TF0
        mov     a,#03Dh
        movx    @r0,a
        mov     dptr,#0C800h
        mov     a,#0
wait_marker:
        jb      P3.2,wait_marker
        movx    @dptr,a
        mov     a,#03Ch
        movx    @r0,a
        nop
        movx    a,@r0
wait_low:
        jb      P3.2,wait_low
        movx    @dptr,a
        mov     a,#0D0h
        movx    @r0,a
        nop
        movx    a,@r0
wait_high:
        jb      P3.2,wait_high
        movx    @dptr,a
        mov     a,#03Dh
        movx    @r0,a
        ret
