#include <labtool48/labtool48.h>
#include "internal.h"
#include <string.h>

/* Dataman 13.10 0045f7b0..0045ff2f. The generated host microcode calls
 * the selected family's real waves; it never substitutes electrical pulses.
 * Cache bytes are held in the machine's snapshotted host image so replay after
 * a capture cannot repeat mutations in the owning session. */
static void put32(unsigned char *p,uint32_t v) {
    for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));
}
static lt48_status parameter(lt48_compile_variable *v,size_t n,unsigned id,unsigned value) {
    for(size_t i=0;i<n;i++)if(v[i].variable_id==id){
        if(v[i].kind==1)v[i].channel=(uint16_t)value;
        else v[i].value=value;
        return LT48_OK;
    }
    return LT48_FORMAT_ERROR;
}
lt48_status lt48_epm_legacy_step_internal(unsigned op,unsigned arg,
    unsigned char b[][32],lt48_compile_variable *v,size_t n,unsigned char *s) {
    unsigned char id[10]={0};
    static const unsigned be[8]={100,200,300,400,600,1000,2000,5000};
    static const unsigned pg[32]={20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,340,360,380,400,440,480,520,560,600,660,720,800,900,1000,1300,1600,2000};
    switch(op){
    case 0: memset(s,0,LT48_EPM_LEGACY_STATE_SIZE);return LT48_OK;
    case 1: memset(s,0,400);memset(s+800,0,10);return LT48_OK;
    case 2: if(arg>=100)return LT48_FORMAT_ERROR;memcpy(s+arg*4,b[5],4);return LT48_OK;
    case 3: if(b[2][0]>=100)return LT48_FORMAT_ERROR;memcpy(b[5],s+b[2][0]*4,4);return LT48_OK;
    case 4: if(b[2][0]>=100)return LT48_FORMAT_ERROR;put32(s+b[2][0]*4,b[4][0]|(unsigned)b[4][1]<<8);return LT48_OK;
    case 5: memcpy(s+810,b[4],4);return LT48_OK;
    case 6: if(arg>=100)return LT48_FORMAT_ERROR;memcpy(b[4],s+arg*4,2);return LT48_OK;
    case 7: memcpy(b[4],s+810,2);memset(s,0,400);return LT48_OK;
    case 8:
        for(unsigned i=0;i<80;i++)id[i/8]|=(s[4*i]&1u)<<(i%8);
        memcpy(s+800,id,10);break;
    case 9: {
        memcpy(s+400,s,320);
        for(unsigned plane=0;plane<4;plane++){
            unsigned value=0;
            for(unsigned i=0;i<16;i++)value|=((s[(75-5*i)*4]>>plane)&1u)<<i;
            id[2+plane*2]=(unsigned char)(value>>8);id[3+plane*2]=(unsigned char)value;
        }
        unsigned value=0;
        for(unsigned i=0;i<16;i++)value|=(s[(76-5*i)*4]&1u)<<i;
        id[1]=(unsigned char)(((value>>5)&7)^(value<<3));
        id[0]=(unsigned char)(((value>>13)&7)^((value>>8)<<3));
        break;
    }
    case 12: {
        /* 0045FF30 / 00460220: four ID planes plus parameter plane 4.
         * 7160's callback also invalidates the row cache and keeps VPG fixed. */
        memcpy(s+400,s,320);
        for(unsigned plane=0;plane<4;plane++){
            unsigned value=0;
            for(unsigned i=0;i<16;i++)value|=((s[(75-5*i)*4]>>plane)&1u)<<i;
            id[2+plane*2]=(unsigned char)(value>>8);
            id[3+plane*2]=(unsigned char)value;
        }
        unsigned value=0;
        for(unsigned i=0;i<16;i++)value|=((s[(75-5*i)*4]>>4)&1u)<<i;
        id[1]=(unsigned char)(((value>>5)&7)^(value<<3));
        id[0]=(unsigned char)(((value>>13)&7)^((value>>8)<<3));
        if(arg)memset(s,255,400);
        break;
    }
    case 10: memcpy(b[4],s+800,10);return LT48_OK;
    case 11: if(arg>=100)return LT48_FORMAT_ERROR;b[4][0]=s[400+arg*4];return LT48_OK;
    default:return LT48_UNSUPPORTED;
    }
    if(memcmp(id+2,"ALTERA",6))return LT48_DEVICE_MISMATCH;
    lt48_status status=parameter(v,n,9,be[id[0]&7]);
    if(status==LT48_OK)status=parameter(v,n,8,pg[id[0]>>3]);
    if(status==LT48_OK && !(op==12 && arg))status=parameter(v,n,7,1125+25*(id[1]&7));
    return status;
}

lt48_status lt48_epm_legacy_compile_internal(unsigned callback,lt48_compiled_wave **out) {
    uint16_t w[2048]={0};size_t n=1;
#define EMIT(x) do { if(n>=2048)return LT48_FORMAT_ERROR;w[n++]=(x); } while(0)
#define STEP(a,b) do{EMIT(0xf000);EMIT(a);EMIT(b);}while(0)
#define CALL(a) do{EMIT(0x25);EMIT(a);}while(0)
#define RETURN() do{EMIT(0x52);EMIT(1);}while(0)
    switch(callback){
    case 0: STEP(0,0);break;
    case 1: break; /* state has session ownership; original frees three buffers */
    case 2: case 8: STEP(1,0);CALL(18);STEP(callback==2?8:9,0);break;
    case 11: case 13: STEP(1,0);CALL(18);STEP(12,callback==13);break;
    case 3: case 9: case 12: {
        EMIT(callback==12?0xf003:0xf001);size_t branch=n;EMIT(0);
        CALL(8);
        for(unsigned col=0;col<5;col++){
            for(int row=15;row>=0;row--){CALL(12);STEP(2,row*5+col);CALL(14);}
            CALL(10);CALL(11);
        }
        CALL(10);RETURN();w[branch]=(uint16_t)n;
        CALL(9);
        if(callback==3){
            for(int i=79;i>=9;i-=10)for(int j=0;j<2;j++){CALL(12);STEP(2,i-j*9);CALL(14);}
        }else{
            for(int i=75;i>=0;i-=5){CALL(12);STEP(2,i);CALL(14);}
        }
        CALL(10);break;
    }
    case 4: STEP(3,0);break;
    case 5: STEP(4,0);break;
    case 6:
        STEP(5,0);
        for(int i=79;i>=0;i--){STEP(6,i);CALL(13);}
        STEP(7,0);break;
    case 7: STEP(10,0);break;
    case 10: for(int i=79;i>=0;i--){STEP(11,i);CALL(13);}break;
    default:return LT48_UNSUPPORTED;
    }
    RETURN();w[0]=(uint16_t)(n-1);
    return lt48_compiled_host_words_internal(w,n,out);
#undef EMIT
#undef STEP
#undef CALL
#undef RETURN
}
