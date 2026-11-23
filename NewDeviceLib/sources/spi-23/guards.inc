/* BP protection is inapplicable when permanent WPSEL selects sector mode. */
static lt48_status nx_secure_guard(const struct as_closure*p,int secure,
 const lt48_runtime_configuration_value*values,size_t count){
 int selected=0;for(size_t i=0;i<sizeof(nx_closures)/sizeof(*nx_closures);i++)if(p==&nx_closures[i])selected=1;
 if(!selected||!secure||p->configuration!=25208)return LT48_OK;
 for(size_t i=0;i<count;i++)if(values[i].category==0&&values[i].field==8)
  return values[i].value?LT48_SAFETY_REJECTED:LT48_OK;
 return LT48_FORMAT_ERROR;
}
