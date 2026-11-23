/* Authored source-bound ID voltage correction. PM0593 Rev1.4 p4: VID10V +/-0.5V.
 * Source M35 sets VID12V. Only the exact expected variable is changed. */
static lt48_status mx27pageone_id_voltage(lt48_compile_variable*v,size_t n){
 unsigned matches=0;for(size_t i=0;i<n;i++)if(v[i].kind==0&&v[i].variable_id==4){if(v[i].value!=1200||v[i].channel!=3)return LT48_SAFETY_REJECTED;v[i].value=1000;matches++;}
 return matches==1?LT48_OK:LT48_SAFETY_REJECTED;
}
