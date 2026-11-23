#!/usr/bin/env python3
"""Bound exact imported applet plus authored timeout return; no publication."""
from hashlib import sha256
BASE_SHA='eb9e9465f27f3aebaa86c2b2fa718ff1b9f9e8ccc086378de7540c8e1dfb9e30'
# Byte encoding of timeout.asm; independently checked in the instruction audit.
TAIL=bytes.fromhex('c28c c28d 743d f2 90c800 7400 20b2fd f0 743c f2 00 e2 20b2fd f0 74d0 f2 00 e2 20b2fd f0 743d f2 22')
def build(original):
    if len(original)!=180 or sha256(original).hexdigest()!=BASE_SHA:
        raise ValueError('Wrong imported Atmel applet')
    patched=bytearray(original)
    for offset in (0x76,0x9b):
        if patched[offset:offset+3]!=bytes.fromhex('025300'):
            raise ValueError('Timeout branch mismatch')
        patched[offset:offset+3]=bytes.fromhex('0251b4')
    return bytes(patched)+TAIL
