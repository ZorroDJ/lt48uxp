/* Authored finite bindings to unchanged BYO Sharp procedures. */
static const struct gaaf_profile sharpwordseven_profiles[]={
{214,197,66,2097152,{9,17,297,299,16,296,301,300,303,302,65535,65535,295,305,249,246,65535,248,65535,65535,65535}},
{201,186,59,2097152,{9,17,260,251,259,258,253,257,256,255,65535,65535,264,250,225,246,65535,248,65535,65535,65535}},
{207,190,62,4194304,{9,17,271,266,270,269,267,273,272,268,65535,65535,264,274,225,246,65535,248,65535,65535,65535}},
{208,191,63,4194304,{9,17,271,275,270,269,276,273,272,277,65535,65535,263,274,225,246,65535,248,65535,65535,65535}},
{217,200,68,2097152,{9,17,271,307,270,306,308,311,310,309,65535,65535,313,312,225,246,65535,248,65535,65535,65535}},
{212,195,62,4194304,{9,17,260,285,259,286,289,288,287,291,65535,65535,293,274,225,246,65535,248,65535,65535,65535}},
{213,196,62,4194304,{9,17,260,285,259,286,290,288,287,292,65535,65535,294,274,225,246,65535,248,65535,65535,65535}},
};
static int sharpwordseven_key(const lt48_catalogue_entry*e){if(e->device_class!=2003||strcmp(e->vendor,"SHARP"))return 0;
if(e->device_type==0xd6&&e->device_name_length==15&&!memcmp(e->device_name,"LRS1316A @72BGA",15))return 1;
if(e->device_type==0xc9&&e->device_name_length==16&&!memcmp(e->device_name,"LRS1331 (Minato)",16))return 1;
if(e->device_type==0xcf&&e->device_name_length==16&&!memcmp(e->device_name,"LRS1337 (Minato)",16))return 1;
if(e->device_type==0xd0&&e->device_name_length==16&&!memcmp(e->device_name,"LRS1340 (Minato)",16))return 1;
if(e->device_type==0xd9&&e->device_name_length==16&&!memcmp(e->device_name,"LRS1356 (Minato)",16))return 1;
if(e->device_type==0xd4&&e->device_name_length==16&&!memcmp(e->device_name,"LRS1362 (Minato)",16))return 1;
if(e->device_type==0xd5&&e->device_name_length==16&&!memcmp(e->device_name,"LRS1363 (Minato)",16))return 1;
return 0;}
