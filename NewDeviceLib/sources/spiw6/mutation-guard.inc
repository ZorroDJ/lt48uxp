/* Advanced per-sector protection is outside the admitted BP workflow. Read
 * both source status banks before any mutation, including source chip Erase. */
static lt48_status sw_mutation_guard(as_session*s,lt48_operation_kind op){
 if(!lt48_addtab_spi_is_spiw6_internal(s->r)||s->r->closure->definition<0x133||
    (op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_OVER_PROGRAM&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE))return LT48_OK;
 unsigned mode=s->mode;s->mode=0;set32(s->buffers[0],0);set32(s->buffers[2],s->r->capacity);
 lt48_status st=run(s,2);if(!st)st=run(s,6);unsigned seen=0;
 if(!st)for(size_t i=0;i<s->config_count;i++)if(s->config[i].category==0&&(s->config[i].field==8||s->config[i].field==264)){
  seen|=s->config[i].field==8?1:2;if(s->config[i].value)st=LT48_SAFETY_REJECTED;
 }
 if(!st&&seen!=3)st=LT48_PROTOCOL_ERROR;
 lt48_status reset=run(s,3);if(!st)st=reset;s->mode=mode;return st;
}
