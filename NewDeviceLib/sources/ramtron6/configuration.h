static const struct numicro_field r6_fields[]={
{0,0,1,"Apply code protection (write action)",{NULL}},
{2,0,8,"Boot-area Fuse setting",{NULL}},
};
static const struct numicro_field*r6_schema(unsigned id,unsigned*n){if(id!=25102)return NULL;*n=2;return r6_fields;}
