/* Source GADA VerifyVCC0/1 presence, not editor enable, selects fixed slots. */
static lt48_status otp13five_verify_voltages(const lt48_mcu51_resources*r,unsigned type,uint32_t out[4],unsigned*count){
 uint32_t values[4]={0};unsigned present=0;
 if(!r||!r->l14||(r->l14->kind!=8&&r->l14->kind!=9))return LT48_UNSUPPORTED;
 if(r->l14->kind==9)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].kind==0&&(r->vars[i].variable_id==3||r->vars[i].variable_id==4)){unsigned slot=r->vars[i].variable_id-3;values[slot]=r->vars[i].value;present|=1u<<slot;}
 if(r->l14->kind==9&&present!=3)return LT48_FORMAT_ERROR;
 return lt48_pic_source_verify_voltages(r->nominal,values,present,type,0,out,count);
}
