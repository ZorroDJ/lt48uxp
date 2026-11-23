#define _XOPEN_SOURCE 700
#include <labtool48/labtool48.h>
#include "internal.h"
#include "json_reader_internal.h"
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef LT48_NEW_DEVICE_LIB_SOURCE
#define LT48_NEW_DEVICE_LIB_SOURCE ""
#endif
#ifndef LT48_NEW_DEVICE_LIB_INSTALL
#define LT48_NEW_DEVICE_LIB_INSTALL "/usr/local/share/labtool48/NewDeviceLib"
#endif

typedef struct nd_resource {
    char id[128], version[48], kind[32], path[512], sha256[65], abi[128];
} nd_resource;
struct lt48_new_device_lib {
    char root[4096];
    lt48_new_device_record *records;
    nd_resource *resources;
    size_t count, resource_count;
};

static int nd_text(const json_parser *p,size_t obj,const char *key,char *dst,size_t n) {
    return token_text(p,object_find(p,obj,key),dst,n);
}
static int nd_uint(const json_parser *p,size_t obj,const char *key,uint32_t *dst) {
    uint64_t v;
    if(!token_uint64(p,object_find(p,obj,key),&v)||v>UINT32_MAX)return 0;
    *dst=(uint32_t)v;return 1;
}
static void nd_hex(const unsigned char *v,char *out) {
    for(unsigned i=0;i<32;i++)snprintf(out+2*i,3,"%02x",v[i]);
}
/* No traversal or symlink escape, including the last component. */
static int nd_path(const lt48_new_device_lib *l,const char *relative,char out[4096]) {
    if(!relative[0]||relative[0]=='/'||strchr(relative,'\\')||strchr(relative,':'))return 0;
    for(const char *p=relative;*p;) {
        const char *end=strchr(p,'/');size_t n=end?(size_t)(end-p):strlen(p);
        if(!n||(n==1&&p[0]=='.')||(n==2&&p[0]=='.'&&p[1]=='.'))return 0;
        if(!end)break;
        p=end+1;
        if(!*p)return 0;
    }
    char joined[4096];int n=snprintf(joined,sizeof joined,"%s/%s",l->root,relative);
    if(n<0||(size_t)n>=sizeof joined||!realpath(joined,out))return 0;
    size_t root_len=strlen(l->root);
    return !strncmp(out,l->root,root_len)&&out[root_len]=='/';
}
static int nd_load(const lt48_new_device_lib *l,const char *relative,const char *expected,
                   unsigned char **data,size_t *size) {
    char path[4096],hex[65];unsigned char hash[32];
    if(!nd_path(l,relative,path)||read_file(path,data,size)!=LT48_OK)return 0;
    lt48_sha256(*data,*size,hash);nd_hex(hash,hex);
    if(expected&&strcmp(hex,expected)){free(*data);*data=NULL;return 0;}
    return 1;
}
static int nd_parse(json_parser *p,unsigned char *data,size_t size) {
    *p=(json_parser){.data=data,.size=size};
    if(!parse_value(p,SIZE_MAX))return 0;
    skip_space(p);return p->cursor==size&&!p->failed;
}
static int nd_ref(const lt48_new_device_lib *l,const char *id) {
    for(size_t i=0;i<l->resource_count;i++)if(!strcmp(id,l->resources[i].id))return (int)i;
    return -1;
}
static int nd_refs(const lt48_new_device_lib *l,const json_parser *p,size_t a,int evidence) {
    size_t n=array_count(p,a);if(n==SIZE_MAX||n>1024)return 0;
    for(size_t i=0;i<n;i++) {
        char id[128];if(!token_text(p,array_item(p,a,i),id,sizeof id))return 0;
        int ref=nd_ref(l,id);if(ref<0||(evidence&&strcmp(l->resources[ref].kind,"evidence")))return 0;
    }
    return 1;
}
static int nd_record(lt48_new_device_lib *l,const json_parser *p,lt48_new_device_record *r) {
    static const char *const fields[]={"id","version","kind","status","manufacturer","part","base","overrides","variables","resources","evidence"};
    static const char *const base_fields[]={"key","file","sha256","class","definition","family","pinmap","pins","capacity_bytes"};
    static const char *const override_fields[]={"device_type","definition","pinmap","pins","capacity_bytes","manufacturer_id","device_id"};
    size_t b=object_find(p,0,"base"),o=object_find(p,0,"overrides"),v=object_find(p,0,"variables");
    if(!object_fields(p,0,fields,11)||!object_fields(p,b,base_fields,sizeof base_fields/sizeof *base_fields))return 0;
#define TXT(k,f) if(!nd_text(p,0,k,r->f,sizeof r->f))return 0
    TXT("id",id);TXT("version",version);TXT("kind",kind);TXT("status",status);TXT("manufacturer",manufacturer);TXT("part",part);
#undef TXT
    if(!valid_identifier(p,object_find(p,0,"id"))||!valid_version(p,object_find(p,0,"version")))return 0;
    if(strcmp(r->kind,"derived")&&strcmp(r->kind,"correction")&&strcmp(r->kind,"experimental"))return 0;
    if(strcmp(r->status,"audited")&&strcmp(r->status,"hardware-qualified")&&strcmp(r->status,"experimental"))return 0;
    if(!strcmp(r->kind,"experimental")&&strcmp(r->status,"experimental"))return 0;
#define BTXT(k,f) if(!nd_text(p,b,k,r->f,sizeof r->f))return 0
    BTXT("key",base_key);BTXT("file",base_file);BTXT("sha256",base_sha256);
#undef BTXT
    if(!valid_sha256(p,object_find(p,b,"sha256"))||strchr(r->base_file,'/')||strchr(r->base_file,'\\')||strstr(r->base_file,".."))return 0;
#define NUM(k,f) if(!nd_uint(p,b,k,&r->f))return 0
    NUM("class",base_class);NUM("definition",base_definition);NUM("family",base_family);NUM("pinmap",base_pinmap);NUM("pins",base_pins);NUM("capacity_bytes",base_capacity);
#undef NUM
    if(r->base_class>65535||r->base_definition>65535||r->base_family>65535||r->base_pinmap>65535||!r->base_pins||r->base_pins>48||!r->base_capacity)return 0;
    if(o>=p->count||p->tokens[o].type!=JSON_OBJECT)return 0;
    uint32_t *dest[]={&r->device_type,&r->definition,&r->pinmap,&r->pins,&r->capacity,&r->manufacturer_id,&r->device_id};
    unsigned seen=0;int key=1;
    for(size_t i=o+1;i<p->count&&p->tokens[i].start<p->tokens[o].end;i++) {
        if(p->tokens[i].parent!=o)continue;
        if(!key){key=1;continue;}key=0;unsigned j;
        for(j=0;j<7;j++)if(token_equal(p->data,&p->tokens[i],override_fields[j]))break;
        if(j==7||(seen&(1u<<j))||!nd_uint(p,o,override_fields[j],dest[j]))return 0;
        seen|=1u<<j;
        if((j!=4&&*dest[j]>65535)||(j==3&&(!*dest[j]||*dest[j]>48))||(j==4&&!*dest[j]))return 0;
    }
    r->override_mask=seen;
    r->variable_count=array_count(p,v);if(r->variable_count>32)return 0;
    for(size_t i=0;i<r->variable_count;i++) {
        static const char *const vf[]={"id","kind","expected","value"};
        size_t q=array_item(p,v,i);uint32_t id,kind;
        if(!object_fields(p,q,vf,4)||!nd_uint(p,q,"id",&id)||!nd_uint(p,q,"kind",&kind)||id>65535||(kind!=0&&kind!=2)||
           !nd_uint(p,q,"expected",&r->variables[i].expected)||!nd_uint(p,q,"value",&r->variables[i].value))return 0;
        r->variables[i].id=id;r->variables[i].kind=kind;
        for(size_t j=0;j<i;j++)if(r->variables[j].id==id)return 0;
    }
    if(!strcmp(r->kind,"correction")&&(seen||!r->variable_count))return 0;
    /* Operational deltas must have an audited consumer: AddTab SPI or the
       exact ATmega signature-only cohort in the native AVR owner. */
    int avr_signature=r->base_class==2010&&!strcmp(r->base_file,"GAB____.rez")&&
        !strcmp(r->base_sha256,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9")&&
        r->base_definition>=0x116&&r->base_definition<=0x12a&&r->variable_count==1&&
        r->variables[0].kind==2&&r->variables[0].id==40&&r->variables[0].expected<=255&&r->variables[0].value<=255;
    int avr_usb_signature=r->base_class==2010&&!strcmp(r->base_file,"GAB____.rez")&&
        !strcmp(r->base_sha256,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9")&&
        r->base_definition==0xc8&&r->base_family==0x9f&&r->base_pinmap==0x41&&r->base_pins==32&&r->base_capacity==135168&&r->variable_count==1&&
        r->variables[0].kind==2&&r->variables[0].id==40&&r->variables[0].expected==0x81&&r->variables[0].value==0x82;
    int avr_voltage=r->base_class==2010&&!strcmp(r->base_file,"GAB____.rez")&&
        !strcmp(r->base_sha256,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9")&&
        r->base_definition>=0x113&&r->base_definition<=0x115&&r->variable_count==1&&
        r->variables[0].kind==0&&r->variables[0].id==0&&
        r->variables[0].expected==(330u|(2u<<16))&&r->variables[0].value==(500u|(2u<<16));
    int avr35_base=r->base_class==2010&&!strcmp(r->base_file,"GAB____.rez")&&
        !strcmp(r->base_sha256,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9")&&r->variable_count==1;
    int avr35_signature=avr35_base&&r->base_definition==0x50&&r->base_family==61&&r->base_pinmap==12&&r->base_pins==8&&r->base_capacity==2176&&
        r->variables[0].kind==2&&r->variables[0].id==40&&r->variables[0].expected==3&&r->variables[0].value==6;
    int avr35_voltage=avr35_base&&r->base_definition==0xcc&&r->base_family==163&&r->base_pinmap==68&&r->base_pins==20&&r->base_capacity==2176&&
        r->variables[0].kind==0&&r->variables[0].id==0&&r->variables[0].expected==(330u|(2u<<16))&&r->variables[0].value==(500u|(2u<<16));
    int spi=r->base_class==2044;for(size_t i=0;i<r->variable_count;i++)if(r->variables[i].kind!=2)spi=0;
    if(r->variable_count&&(strcmp(r->kind,"correction")||(!spi&&!avr_signature&&!avr_usb_signature&&!avr_voltage&&!avr35_signature&&!avr35_voltage)))return 0;
    return nd_refs(l,p,object_find(p,0,"resources"),0)&&nd_refs(l,p,object_find(p,0,"evidence"),1)&&array_count(p,object_find(p,0,"evidence"))>0;
}

void lt48_new_device_lib_close(lt48_new_device_lib *l) {
    if(l){free(l->records);free(l->resources);free(l);}
}
size_t lt48_new_device_lib_count(const lt48_new_device_lib *l){return l?l->count:0;}
const lt48_new_device_record *lt48_new_device_lib_record(const lt48_new_device_lib *l,size_t i){return l&&i<l->count?&l->records[i]:NULL;}

lt48_status lt48_new_device_lib_open(const char *root,lt48_new_device_lib **out,lt48_new_device_lib_report *report) {
    lt48_new_device_lib_report local={0};if(!report)report=&local;memset(report,0,sizeof *report);
    if(!root||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    lt48_new_device_lib *l=calloc(1,sizeof *l);if(!l)return LT48_NO_MEMORY;
    unsigned char *data=NULL;size_t size=0;json_parser p={0};lt48_status st=LT48_FORMAT_ERROR;
#define REQUIRE_ND(c,msg) do{if(!(c)){snprintf(report->diagnostic,sizeof report->diagnostic,"%s",msg);goto fail;}}while(0)
    REQUIRE_ND(realpath(root,l->root),"NewDeviceLib root is missing");
    snprintf(report->root,sizeof report->root,"%s",l->root);
    REQUIRE_ND(nd_load(l,"library.json",NULL,&data,&size),"Cannot read library.json");
    unsigned char hash[32];lt48_sha256(data,size,hash);nd_hex(hash,report->sha256);
    REQUIRE_ND(nd_parse(&p,data,size),"Malformed library JSON");
    static const char *const top[]={"schema_version","id","version","records","resources","bundles"};uint32_t schema;
    REQUIRE_ND(object_fields(&p,0,top,6)&&nd_uint(&p,0,"schema_version",&schema)&&schema==1&&valid_identifier(&p,object_find(&p,0,"id"))&&valid_version(&p,object_find(&p,0,"version")),"Unsupported library schema");
    size_t rr=object_find(&p,0,"resources"),dd=object_find(&p,0,"records"),bb=object_find(&p,0,"bundles");
    l->count=array_count(&p,dd);l->resource_count=array_count(&p,rr);report->bundles=array_count(&p,bb);
    REQUIRE_ND(l->count<=4096&&l->resource_count<=4096&&report->bundles<=1024,"Invalid or oversized library arrays");
    l->records=calloc(l->count?l->count:1,sizeof *l->records);l->resources=calloc(l->resource_count?l->resource_count:1,sizeof *l->resources);
    REQUIRE_ND(l->records&&l->resources,"Allocation failure");
    for(size_t i=0;i<l->resource_count;i++) {
        nd_resource *r=&l->resources[i];size_t q=array_item(&p,rr,i);
        static const char *const rf[]={"id","version","kind","path","sha256","abi","origin","license","sources","build"};
        char origin[32],license[128],build[128];
        REQUIRE_ND(object_fields(&p,q,rf,10)&&valid_identifier(&p,object_find(&p,q,"id"))&&valid_version(&p,object_find(&p,q,"version"))&&valid_sha256(&p,object_find(&p,q,"sha256")),"Invalid resource identity/schema");
#define RT(k,f) REQUIRE_ND(nd_text(&p,q,k,r->f,sizeof r->f),"Invalid resource text")
        RT("id",id);RT("version",version);RT("kind",kind);RT("path",path);RT("sha256",sha256);RT("abi",abi);
#undef RT
        REQUIRE_ND(nd_text(&p,q,"origin",origin,sizeof origin)&&!strcmp(origin,"authored")&&nd_text(&p,q,"license",license,sizeof license)&&nd_text(&p,q,"build",build,sizeof build),"Only authored resources belong in NewDeviceLib");
        REQUIRE_ND(!strcmp(r->kind,"c-source")||!strcmp(r->kind,"verilog-source")||!strcmp(r->kind,"assembly-source")||!strcmp(r->kind,"80c32-applet")||!strcmp(r->kind,"fpga-bitstream")||!strcmp(r->kind,"build-recipe")||!strcmp(r->kind,"evidence"),"Unknown resource kind");
        for(size_t j=0;j<i;j++)REQUIRE_ND(strcmp(l->resources[j].id,r->id),"Duplicate resource id");
        unsigned char *bytes=NULL;size_t len;
        REQUIRE_ND(nd_load(l,r->path,r->sha256,&bytes,&len),"Resource path/hash mismatch");free(bytes);
    }
    for(size_t i=0;i<l->resource_count;i++) {
        size_t q=array_item(&p,rr,i);char build[128];nd_text(&p,q,"build",build,sizeof build);
        REQUIRE_ND(nd_refs(l,&p,object_find(&p,q,"sources"),0),"Missing source reference");
        int payload=!strcmp(l->resources[i].kind,"80c32-applet")||!strcmp(l->resources[i].kind,"fpga-bitstream");
        int ref=nd_ref(l,build);
        REQUIRE_ND((!payload&&!strcmp(build,"none"))||(ref>=0&&!strcmp(l->resources[ref].kind,"build-recipe")),"Missing payload build recipe");
        REQUIRE_ND(!payload||(array_count(&p,object_find(&p,q,"sources"))>0&&strcmp(l->resources[i].abi,"none")),"Payload requires source references and ABI");
    }
    for(size_t i=0;i<l->count+report->bundles;i++) {
        int bundle=i>=l->count;size_t q=array_item(&p,bundle?bb:dd,bundle?i-l->count:i);
        static const char *const pf[]={"path","sha256"};char path[512],hex[65];
        REQUIRE_ND(object_fields(&p,q,pf,2)&&nd_text(&p,q,"path",path,sizeof path)&&nd_text(&p,q,"sha256",hex,sizeof hex)&&valid_sha256(&p,object_find(&p,q,"sha256")),"Invalid record/bundle reference");
        unsigned char *bytes=NULL;size_t len;
        REQUIRE_ND(nd_load(l,path,hex,&bytes,&len),"Record/bundle path/hash mismatch");
        if(bundle) {
            char full[4096];lt48_component_validation_report br;int ok=nd_path(l,path,full)&&lt48_component_bundle_validate(full,&br)==LT48_OK;
            free(bytes);REQUIRE_ND(ok,"Invalid Composer bundle");
        } else {
            json_parser dp={0};int ok=nd_parse(&dp,bytes,len)&&nd_record(l,&dp,&l->records[i]);
            free(dp.tokens);free(bytes);REQUIRE_ND(ok,"Invalid device record schema or references");
            snprintf(l->records[i].source_path,sizeof l->records[i].source_path,"%s",path);
            snprintf(l->records[i].source_sha256,sizeof l->records[i].source_sha256,"%s",hex);
            for(size_t j=0;j<i;j++) {
                REQUIRE_ND(strcmp(l->records[j].id,l->records[i].id),"Duplicate device id");
                REQUIRE_ND(strcmp(l->records[j].manufacturer,l->records[i].manufacturer)||strcmp(l->records[j].part,l->records[i].part),"Duplicate device name");
            }
        }
    }
    report->records=l->count;report->resources=l->resource_count;
    snprintf(report->diagnostic,sizeof report->diagnostic,"Validated NewDeviceLib; engine qualification remains separate");
    *out=l;l=NULL;st=LT48_OK;
fail:
    free(p.tokens);free(data);lt48_new_device_lib_close(l);return st;
#undef REQUIRE_ND
}

lt48_status lt48_new_device_lib_read_resource(const lt48_new_device_lib *l,const char *id,const char *kind,const char *abi,unsigned char **data,size_t *size) {
    if(!l||!id||!kind||!abi||!data||!size)return LT48_INVALID_ARGUMENT;
    *data=NULL;*size=0;
    int i=nd_ref(l,id);if(i<0)return LT48_NOT_FOUND;
    const nd_resource *r=&l->resources[i];
    if(strcmp(r->kind,kind)||strcmp(r->abi,abi))return LT48_UNSUPPORTED;
    return nd_load(l,r->path,r->sha256,data,size)?LT48_OK:LT48_SAFETY_REJECTED;
}

static int nd_candidate(const char *root,char *path,size_t capacity) {
    struct stat s;char file[4096];int n=snprintf(file,sizeof file,"%s/library.json",root);
    if(n<0||(size_t)n>=sizeof file||stat(file,&s)||!S_ISREG(s.st_mode)||strlen(root)>=capacity)return 0;
    strcpy(path,root);return 1;
}
lt48_status lt48_new_device_lib_default_path(char *path,size_t capacity) {
    if(!path||!capacity)return LT48_INVALID_ARGUMENT;
    path[0]=0;
    const char *explicit_path=getenv("LT48_NEW_DEVICE_LIB");
    if(explicit_path){if(!explicit_path[0]||strlen(explicit_path)>=capacity)return LT48_INVALID_ARGUMENT;strcpy(path,explicit_path);return LT48_OK;}
    char candidate[4096];const char *xdg=getenv("XDG_DATA_HOME"),*home=getenv("HOME");
    if(xdg&&xdg[0]=='/')snprintf(candidate,sizeof candidate,"%s/labtool48/NewDeviceLib",xdg);
    else if(home)snprintf(candidate,sizeof candidate,"%s/.local/share/labtool48/NewDeviceLib",home);
    else candidate[0]=0;
    if(candidate[0]&&nd_candidate(candidate,path,capacity))return LT48_OK;
#ifdef __linux__
    char exe[4096];ssize_t n=readlink("/proc/self/exe",exe,sizeof exe-1);
    if(n>0){exe[n]=0;char *slash=strrchr(exe,'/');if(slash){*slash=0;
        if(snprintf(candidate,sizeof candidate,"%s/../share/labtool48/NewDeviceLib",exe)<(int)sizeof candidate&&nd_candidate(candidate,path,capacity))return LT48_OK;}}
#endif
    if(nd_candidate(LT48_NEW_DEVICE_LIB_INSTALL,path,capacity))return LT48_OK;
    if(LT48_NEW_DEVICE_LIB_SOURCE[0]&&nd_candidate(LT48_NEW_DEVICE_LIB_SOURCE,path,capacity))return LT48_OK;
    return LT48_NOT_FOUND;
}
