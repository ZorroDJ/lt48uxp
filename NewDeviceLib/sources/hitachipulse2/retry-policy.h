/* Authored policy: honor the selected source RETRY variable for the313-byte
 * worker, whose first FIFO byte comes from _BUF_Para. Source words unchanged.
 * A zero byte would wrap the worker's DJNZ retry counter to256. */
static unsigned hitachipulse2_retry(const lt48_compile_variable*v,size_t n){
 for(size_t i=0;i<n;i++)if(v[i].kind==2&&v[i].variable_id==13&&v[i].value==25&&v[i].channel==0)return 25;
 return 0;
}
