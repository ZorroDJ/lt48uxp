/* Authored correction for exactly GDCA W24f/config25435: checkbox5 is TB,
 * checkbox6 is Factory Lock. The caller has verified the complete source hash.
 * No vendor waveform bytes are embedded beyond the narrow correction guard. */
static lt48_status mx_correct_tb(unsigned char*bytes,size_t length){
 static const unsigned char before[]={0xa2,0,6,0,0x12,0x45,0x19,3,0x12,0,1};
 if(length<0x55+sizeof before||memcmp(bytes+0x55,before,sizeof before))return LT48_SAFETY_REJECTED;
 bytes[0x57]=5;return LT48_OK;
}
