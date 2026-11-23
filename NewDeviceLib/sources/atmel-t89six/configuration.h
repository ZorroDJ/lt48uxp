/* Authored GUI schema for the original named-object field bindings. */
static const struct numicro_field t89_fields_schema[]={
 {0,12,1,"LB0: programming lock",{NULL}},
 {0,13,1,"LB1: read/verify lock",{NULL}},
 {0,14,1,"LB2: highest lock",{NULL}},
 {0,5,1,"Bootloader jump bit programmed",{NULL}},
 {0,4,1,"Start in X2 mode",{NULL}},
 {2,8,255,"Boot Status Byte (BSB)",{NULL}},
 {2,9,255,"Software Boot Vector (SBV)",{NULL}},
 {2,16,255,"P1_CF",{NULL}},
 {2,17,255,"P3_CF",{NULL}},
 {2,18,255,"P4_CF",{NULL}},
 {2,11,255,"Extra Byte (EB)",{NULL}}
};
static const struct numicro_field*t89_schema(unsigned id,unsigned*n){if(id!=60019)return NULL;*n=11;return t89_fields_schema;}
