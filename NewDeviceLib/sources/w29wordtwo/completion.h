/* Authored result capture using the established C800 read transport.
 * Exact source Erase13 already waits tErase=100000us; retain that wait.
 * Program9 can return1 even after32failed polls: raw readback is mandatory. */
static const unsigned char w29wordtwo_ack[]={2,0,0x7c,7};
