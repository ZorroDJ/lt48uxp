# HC705 BD operation owner

Authored C owner; exact Dataman13.10 GADB resources remain bring-your-own.
No original REZ, controller applet or FPGA payload is included.
BD3/BD7 first eight bindings, then BD9 two. Admission is separately gated.
Read, Blank Check, Program, Verify and Compare require whole sparse images.
No electrical Erase or Secure; hardware and analogue qualification pending.

BD3/BD7 blank is00. Their Program stream transfers all16384 image bytes,
including FF through holes; their Read stream is sparse. Each complete C
Program stream must match the original-interpreter census checksum. BD9 blank isFF and its original applet buffers24080 bytes
in controller RAM before/while transferring to the host. Buffer echoes do not
prove target programming: fresh nominal/low/high raw reads are mandatory.
The BD9 USB timeout floor is100seconds, matching the original timeout window.
GTK warns that programming can take another58seconds after data transfer.
Its non-gating source notification is relocated to that preparation message.
If a BD9 buffer acknowledgement is wrong, the owner fails closed without
sending Stop/reset into an incomplete receiver. Cleanup is reported failed;
transport/controller recovery is needed before retrying. A partial Read is
never published after failure. BD3/BD7 RAM-only failure handlers wait for Stop.

See docs/audit-qualification/HC705_BD_PROGRESS.md and immutable audit receipts.
