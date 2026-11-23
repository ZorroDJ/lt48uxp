/* Micron C3 Rev3 recommended VCC is2.7..3.3V, including read margins. */
static unsigned micronc3two_voltage(unsigned v){return v<270?270:v>330?330:v;}
