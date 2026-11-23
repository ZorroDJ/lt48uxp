static const struct numicro_field a91_fields[]={
{0,0,1,"Apply security (write action; erase required to unlock)",{NULL}},
{2,0,65535,"Set additional region lock bits",{NULL}},
{2,1,3,"Set additional general-purpose NVM bits",{NULL}},
};
static const struct numicro_field*a91_schema(unsigned id,unsigned*n){if(id!=25110)return NULL;*n=3;return a91_fields;}
