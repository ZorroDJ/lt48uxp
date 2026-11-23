/* Authored source-bound correction: M39432 has 32KiB EEPROM, not 8KiB. */
#define ST2009TWO_SOURCE_SHA "9fdc0c7a88bb96aaead0314fb4f85c0800ab987abf370908000493cc272d13f6"
static inline unsigned st2009two_geometry(unsigned cl,unsigned d,unsigned f,unsigned p,unsigned cap,const char*sha){
 return cl==2009&&d==0x14&&f==11&&p==4&&cap==532544&&!strcmp(sha,ST2009TWO_SOURCE_SHA)?557120:cap;
}
static inline lt48_status st2009two_variables(lt48_compile_variable*v,size_t n,unsigned d){
 unsigned seen=0;
 for(size_t i=0;i<n;i++)if(v[i].kind==2&&v[i].variable_id==12){
  if(v[i].value!=8192||v[i].channel!=(d==0x14?8:4))return LT48_SAFETY_REJECTED;
  if(d==0x14)v[i].value=32768;
  seen++;
 }return seen==1?LT48_OK:LT48_SAFETY_REJECTED;
}
