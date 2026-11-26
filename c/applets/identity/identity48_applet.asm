; ZAPPLET identity banner, private lt48uxp protocol.
; Load B000. Request, AA55 and response use only EPP C800 with P3.2 wait.
    .ORG 0xb000
ENTRY:
    mov r6, #0x09
    mov r0, #0x20
READ_REQ:
    lcall GETC
    mov @r0, a
    inc r0
    djnz r6, READ_REQ
    mov a, 0x25
    mov 0x0e, a             ; echo nonce
    mov a, 0x20
    cjne a, #0x4c, FAIL_BAD ; L
    mov a, 0x21
    cjne a, #0x54, FAIL_BAD ; T
    mov a, 0x22
    cjne a, #0x49, FAIL_BAD ; I
    mov a, 0x23
    cjne a, #0x44, FAIL_BAD ; D
    mov a, 0x24
    cjne a, #0x01, FAIL_BAD
    mov a, 0x27
    cjne a, #0x00, FAIL_BAD
    mov r5, #0x00
    mov r0, #0x20
    mov r6, #0x08
CHKSUM:
    mov a, @r0
    xrl 5, a
    inc r0
    djnz r6, CHKSUM
    mov a, 0x28
    cjne a, 5, FAIL_BAD
    mov a, 0x26
    cjne a, #0x01, FAIL_UNSUP
    sjmp VALID
FAIL_BAD:
    ljmp BAD
FAIL_UNSUP:
    ljmp UNSUP
VALID:
    lcall FIND_HANDLER
    jnc FALLBACK_START
    ljmp HAVE_HANDLER
FALLBACK_START:
    mov r7, #0x00           ; status OK, fallback banner
    mov r6, #0x27           ; 1 length + 37 text + NUL
    lcall SEND_HEADER
    mov a, #0x26
    lcall EMIT              ; fallback banner length (checksummed)
    mov dptr, #FALLBACK
    mov r6, #0x25
FALLBACK_LOOP:
    clr a
    movc a, @a+dptr
    inc dptr
    xrl 5, a
    djnz r6, FALLBACK_LOOP
    mov 0x0f, 5
    mov r2, #(FALLBACK & 0xff)
    mov r3, #((FALLBACK >> 8) & 0xff)
    mov r6, #0x25
FALLBACK_SEND:
    mov 0x82, r2
    mov 0x83, r3
    clr a
    movc a, @a+dptr
    inc r2
    cjne r2, #0x00, FS_OK
    inc r3
FS_OK:
    lcall EMIT_RAW
    djnz r6, FALLBACK_SEND
    mov a, #0x00
    lcall EMIT_RAW
    ljmp SEND_TAIL

HAVE_HANDLER:
    ; target in R2:R3, length at target+14h
    mov dptr, #0
    mov 0x82, r2
    mov 0x83, r3
    mov a, #0x15
    movc a, @a+dptr
    mov r6, a               ; banner length N
    inc r6                  ; payload includes the length byte
    mov r7, #0x00
    lcall SEND_HEADER
    ; payload checksum: length byte then N banner bytes
    mov 0x82, r2
    mov 0x83, r3
    mov a, #0x15
    movc a, @a+dptr
    xrl 5, a
    mov r0, #0x00
    mov r1, a               ; N
PCSUM:
    mov 0x82, r2
    mov 0x83, r3
    mov a, r0
    add a, #0x24
    movc a, @a+dptr
    xrl 5, a
    inc r0
    djnz r1, PCSUM
    mov 0x0f, 5
    ; call handler, then send checksum and trailer
    mov 0x82, r2
    mov 0x83, r3
    mov a, #(AFTER_CALL & 0xff)
    push acc
    mov a, #((AFTER_CALL >> 8) & 0xff)
    push acc
    push 0x82
    push 0x83
    ret
AFTER_CALL:
    mov r5, 0x0f
    ljmp SEND_TAIL

BAD:
    mov r7, #0x02
    mov r6, #0x00
    lcall SEND_HEADER
    sjmp SEND_TAIL
UNSUP:
    mov r7, #0x01
    mov r6, #0x00
    lcall SEND_HEADER
    sjmp SEND_TAIL
SEND_TAIL:
    mov a, r5
    lcall EMIT_RAW
    mov a, #0xa5
    lcall EMIT_RAW
    mov a, #0x5a
    lcall EMIT_RAW
    ret

SEND_HEADER:
    mov r5, #0x00
    mov a, #0x4c
    lcall EMIT
    mov a, #0x54
    lcall EMIT
    mov a, #0x52
    lcall EMIT
    mov a, #0x53
    lcall EMIT
    mov a, #0x01
    lcall EMIT
    mov a, 0x0e
    lcall EMIT
    mov a, #0x01
    lcall EMIT
    mov a, r7
    lcall EMIT
    mov a, r6
    lcall EMIT
    ret

EMIT:
    xrl 5, a
EMIT_RAW:
    mov dptr, #0xc800
EW:
    jb 0xb0.2, EW
    movx @dptr, a
    ret
GETC:
    mov dptr, #0xc800
GR:
    jb 0xb0.2, GR
    movx a, @dptr
    ret

FIND_HANDLER:
    mov dptr, #0x0000
SCAN:
    mov 0x82, dpl
    mov 0x83, dph
    mov r2, dpl
    mov r3, dph
    mov r1, #0x00
CMP:
    mov dptr, #PATTERN
    mov a, r1
    movc a, @a+dptr
    mov r7, a
    mov 0x82, r2
    mov 0x83, r3
    mov a, r1
    movc a, @a+dptr
    cjne a, 0x07, NEXT
    inc r1
    cjne r1, #0x15, CMP
    setb c
    ret
NEXT:
    mov 0x82, r2
    mov 0x83, r3
    inc dptr
    mov r2, dpl
    mov r3, dph
    mov a, dph
    cjne a, #0x30, SCAN
    clr c
    ret

PATTERN:
    .db 0x90,0xc8,0x00,0x20,0xb2,0xfd,0xe0,0xb4,0xaa,0x19
    .db 0x20,0xb2,0xfd,0xe0,0xb4,0x55,0x12,0x20,0xb2,0xfd,0x74
FALLBACK:
    .db 'L','a','b','T','o','o','l','-','U','N','K','N','O','W','N'
    .db ' ','F','i','r','m','w','a','r','e',' ','V','e','r','s','i','o','n'
    .db ' ','0','.','0','0'
    .END
