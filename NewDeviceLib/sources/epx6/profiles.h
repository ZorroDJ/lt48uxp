/* Authored exact source identities for Altera/Intel PLD table-driven owner. */
static inline int epx6_definition(unsigned d,unsigned m,unsigned p){static const unsigned rows[][3]={
 {4,2,1},
 {5,2,2},
 {6,3,3},
 };for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(d==rows[i][0]&&m==rows[i][1]&&p==rows[i][2])return 1;return 0;}
static inline int epx6_binding(const lt48_device_spec*s){if(s->catalogue.device_class!=2022||strcmp(s->definition_stem,"GBBA___")||!epx6_definition(s->definition_resource,s->family_code,s->pinmap))return 0;
 static const struct{const char*key;unsigned d,m,p,pins;}rows[]={
 {"Altera:EPX740 *44",4,2,1,44},
 {"Altera:EPX740 @68",5,2,2,48},
 {"Altera:EPX780 @84",6,3,3,20},
 {"Intel:iFX780 @84",6,3,3,20},
 {"Intel:iN44FX740 *44",4,2,1,44},
 {"Intel:iN44FX740 @68",5,2,2,48},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins)return 1;return 0;}
