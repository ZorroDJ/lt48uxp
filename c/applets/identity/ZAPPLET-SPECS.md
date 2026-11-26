# ZAPPLET protocol specification

Private applet-side protocol used only by lt48uxp. The USB/parallel adapter
does not parse or rewrite these frames.

## Version 1

Request:

```text
"LTID" version=1 nonce op req_len payload checksum8
```

Response:

```text
"LTRS" version=1 nonce op status resp_len payload checksum8 "A5" "5A"
```

All multi-byte fields are single bytes. `checksum8` is XOR of every preceding
frame byte after the magic included, up to the last payload byte.

- `nonce`: echoed exactly.
- `op`: command number.
- `req_len`, `resp_len`: payload lengths.
- `status`: 0 OK, 1 UNSUPPORTED, 2 BAD_FRAME, 3 RANGE, 4 PERIPHERAL,
  5 BUSY, 6 TIMEOUT.
- Trailer is exactly `A5 5A`.

Maximum request payload: 64. Maximum response payload: 512.

## Implemented command

### `01 BANNER`

Request payload: none (`req_len=0`).

After a valid request the host **reads the 9-byte `LTRS` header first**. Only
then does it send `AA55`, consumed by the resident handler. The applet itself
does not read those two bytes. The header is emitted before the handler runs;
pre-queueing `AA55` while the header is being written can lose bytes on the
real half-duplex EPP bus (an in-memory emulator may not expose this race).

Response payload: the stock monitor 51 sub-frame:

```text
<N> <banner> NUL
```

`N` includes the NUL, so `resp_len = N + 1` and `payload[N] = 0`.
The banner text length is `N - 1`.

The applet scans ROM `0000..2FFF` for the resident 51 handler signature:

```text
90 C8 00 20 B2 FD E0 B4 AA 19 20 B2 FD E0 B4 55 12 20 B2 FD 74
```

On match it reads the banner length at `handler+14h`, then calls the handler.
The handler consumes nothing in this protocol and writes the sub-frame to C800.
The applet writes only the enclosing header, checksum and trailer.

If the signature is absent, status is OK and the payload is:

```text
"LabTool-UNKNOWN Firmware Version 0.00" + NUL
```

`resp_len` still includes the fallback length byte and NUL.

## Invalid request

- bad magic, bad version, bad request checksum, nonzero `req_len` or truncated
  frame -> `BAD_FRAME`;
- unknown op -> `UNSUPPORTED`;
- both have no payload and `resp_len=0`.

The applet always consumes the fixed 9-byte request header before replying.

## Framing rules

- Request bytes travelled one at a time on EPP `C800` with `P3.2` wait.
- Response header, payload, checksum and trailer use the same wait.
- No `51 AA55` command is sent by the host; after receiving the response
  header, it sends only `AA55` and the applet calls the handler internally.
- The applet returns to the monitor after the trailer.

## Future commands

Opcode numbers are stable. New commands add payload rules and capabilities;
old parsers reject unknown ops with `UNSUPPORTED`.
