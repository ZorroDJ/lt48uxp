/* Original high-voltage shutdown first steps channels above5V down to5V.
 * Other AddTab profiles retain their qualified policy. */
static unsigned fw_shutdown_policy(const struct as_closure*c){return c==fw_closures?1u:0u;}
