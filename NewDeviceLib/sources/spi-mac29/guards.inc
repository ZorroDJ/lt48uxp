/* Authored protection-mode guard. WPSEL is permanent and disables BP mode. */
static lt48_status mx_secure_guard(const struct as_closure*p,int secure,
 const lt48_runtime_configuration_value*values,size_t count){
 int selected=0;for(size_t i=0;i<sizeof(mx_closures)/sizeof(*mx_closures);i++)if(p==&mx_closures[i])selected=1;
 if(!selected||!secure||p->configuration==25469)return LT48_OK;
 unsigned field=p->configuration==25296?9:8;
 for(size_t i=0;i<count;i++)if(values[i].category==0&&values[i].field==field)
  return values[i].value?LT48_SAFETY_REJECTED:LT48_OK;
 return LT48_FORMAT_ERROR;
}
