/* BP-based Secure is inapplicable while individual-block WPS mode is selected. */
static lt48_status gw_secure_guard(const struct as_closure*p,int secure,
 const lt48_runtime_configuration_value*values,size_t count){
 int selected=0;for(size_t i=0;i<sizeof(gw_closures)/sizeof(*gw_closures);i++)if(p==&gw_closures[i])selected=1;
 if(!selected||!secure)return LT48_OK;
 unsigned field;
 switch(p->configuration){case 25372:case 25303:field=12;break;
 case 25348:case 25406:case 25459:field=13;break;case 25409:field=11;break;
 default:return LT48_OK;}
 for(size_t i=0;i<count;i++)if(values[i].category==0&&values[i].field==field)
  return values[i].value?LT48_SAFETY_REJECTED:LT48_OK;
 return LT48_FORMAT_ERROR;
}
