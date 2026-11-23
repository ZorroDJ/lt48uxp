#define _POSIX_C_SOURCE 200809L
/* Diagnostic native-C FM16W08 bus engine. No device applet, no prefetch.
 * DIP28 bottom aligned in ZIF48, 1:1 adapter, 8192 bytes. Not GUI-qualified.
 * Each bit sample closes CE inside one FX2 queue packet. Eight independent
 * read cycles per byte trade speed for bounded, inspectable transactions.
 * Shared calibrated DAC ramps are requested, not measured waveforms. */
#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
static volatile sig_atomic_t interrupted;
static int ce_write, top_supply, sink_ground, byte_read, oe_write;
static unsigned setup_us,pulse_us,hold_us;
static void stop(int sig){(void)sig;interrupted=1;}
/* Physical package pin numbers, LSb first; add 10 for ZIF. */
static const unsigned ap[]={10,9,8,7,6,5,4,3,25,24,21,23,2};
static const unsigned dp[]={11,12,13,15,16,17,18,19};
enum {CE=20,OE=22,WE=27};
typedef struct {lt48_programmer*p;lt48_runtime_transport_session*w;unsigned char q[512];size_t n;int powered;} bus;
static lt48_status wr(void*c,uint8_t e,const unsigned char*b,size_t n,size_t*d){lt48_programmer*p=c;return p->bulk(p->backend_context,e,(unsigned char*)b,n,10000,d);}
static lt48_status rd(void*c,uint8_t e,unsigned char*b,size_t n,size_t*d){lt48_programmer*p=c;return p->bulk(p->backend_context,e,b,n,10000,d);}
static lt48_status control(bus*b,const unsigned char*p,size_t n){lt48_status s=lt48_runtime_transport_session_wave_watchdog(b->w,5000,0);if(s==LT48_OK)s=lt48_runtime_transport_session_controller_write(b->w,p,n);if(s==LT48_OK)s=lt48_runtime_transport_session_flush(b->w);if(s!=LT48_OK)fprintf(stderr,"control %02x/%zu: %s\n",p[0],n,lt48_status_string(s));return s;}
static lt48_status delay(bus*b,unsigned us){unsigned t=us/100;unsigned char p[]={0x4f,t,t>>8};return control(b,p,sizeof p);}
static void begin(bus*b){memset(b->q,0,sizeof b->q);b->q[0]=0x2c;b->q[1]=4;b->q[2]=0;b->n=3;}
static void pair(bus*b,unsigned kind,unsigned value){if(b->n+2>=sizeof b->q)abort();b->q[b->n++]=kind;b->q[b->n++]=value;}
static void pin(bus*b,unsigned package,unsigned state){pair(b,2,4*(package+9)+state);}
static lt48_status send(bus*b,unsigned char*value){unsigned char r[512]={0};size_t n=0;lt48_status s=wr(b->p,2,b->q,512,&n);if(s==LT48_OK&&n!=512)s=LT48_PROTOCOL_ERROR;if(s==LT48_OK)s=rd(b->p,0x88,r,512,&n);if(s==LT48_OK&&(n!=512||!(r[0]|r[1])))s=LT48_PROTOCOL_ERROR;if(s==LT48_OK&&value)*value=r[2]&1;if(s!=LT48_OK)fprintf(stderr,"pin queue: %s ack=%02x%02x n=%zu\n",lt48_status_string(s),r[0],r[1],n);return s;}
static void address(bus*b,unsigned a){for(unsigned k=0;k<13;k++)pin(b,ap[k],(a>>k)&1);}
/* Resident 4E delay remains inside the same FX2 queue: no host wait
 * while CE is active. Values are nominal microseconds, not scope readings. */
static void timing_delay(bus*b,unsigned us){if(us){pair(b,1,0x4e);pair(b,1,us);}}
static void make_write(bus*b,unsigned a,unsigned value){
 begin(b);pin(b,CE,1);pin(b,OE,1);pin(b,WE,1);address(b,a);
 if(oe_write){
  /* Page-9 entry, with OE disabled before WE rises: a sequential pin
   * queue cannot release eight pads inside tWX(min)=10ns. The matched
   * high-OE control uses exactly the same late data drive and delays. */
  for(unsigned k=0;k<8;k++)pin(b,dp[k],2);
  pin(b,OE,oe_write==2?0:1);pin(b,CE,0);timing_delay(b,2);
  pin(b,WE,0);timing_delay(b,2); /* exceed tWZ(max)=15ns */
  for(unsigned k=0;k<8;k++)pin(b,dp[k],(value>>k)&1);
  timing_delay(b,2);timing_delay(b,setup_us);timing_delay(b,pulse_us);
  pin(b,OE,1);timing_delay(b,2);pin(b,WE,1);
  timing_delay(b,2);timing_delay(b,hold_us);
  for(unsigned k=0;k<8;k++)pin(b,dp[k],2);
  pin(b,CE,1);return;
 }
 for(unsigned k=0;k<8;k++)pin(b,dp[k],(value>>k)&1);
 timing_delay(b,setup_us);
 if(ce_write){pin(b,WE,0);pin(b,CE,0);timing_delay(b,pulse_us);pin(b,CE,1);pin(b,WE,1);}
 else {pin(b,CE,0);pin(b,WE,0);timing_delay(b,pulse_us);pin(b,WE,1);pin(b,CE,1);}
 timing_delay(b,hold_us);
 for(unsigned k=0;k<8;k++)pin(b,dp[k],2);
}
static lt48_status write_byte(bus*b,unsigned a,unsigned value){
 if(a>=8192)return LT48_INVALID_ARGUMENT;
 if(interrupted)return LT48_CANCELLED;
 make_write(b,a,value);return send(b,NULL);
}
static void make_read_bit(bus*b,unsigned a,unsigned k){
 begin(b);pin(b,CE,1);pin(b,OE,1);pin(b,WE,1);address(b,a);
 for(unsigned j=0;j<8;j++)pin(b,dp[j],2);
 pin(b,CE,0);pin(b,OE,0);pin(b,dp[k],2);pair(b,3,0);
 pin(b,OE,1);pin(b,CE,1);
}
/* Existing FX2 command09 samples eight released data pads. The CE cycle
 * spans three USB exchanges here; no next address is accessed. Always issue
 * deselection even if capture fails. This is diagnostic, not a bounded
 * firmware-resident pulse and not admitted for FM1608's 10us maximum. */
static lt48_status read_whole_byte(bus*b,unsigned a,unsigned char*value){
 begin(b);pin(b,CE,1);pin(b,OE,1);pin(b,WE,1);address(b,a);
 for(unsigned k=0;k<8;k++)pin(b,dp[k],2);
 pin(b,CE,0);pin(b,OE,0);
 lt48_status s=send(b,NULL);
 unsigned char request[512]={9,8},response[512]={0};size_t n=0;
 for(unsigned k=0;k<8;k++)request[k+2]=4*(dp[k]+9)+2;
 if(s==LT48_OK)s=wr(b->p,2,request,sizeof request,&n);
 if(s==LT48_OK&&n!=512)s=LT48_PROTOCOL_ERROR;
 if(s==LT48_OK)s=rd(b->p,0x88,response,sizeof response,&n);
 if(s==LT48_OK&&(n!=512||!(response[0]|response[1])))s=LT48_PROTOCOL_ERROR;
 *value=0;if(s==LT48_OK)for(unsigned k=0;k<8;k++){
  if(response[k+2]>1){s=LT48_PROTOCOL_ERROR;break;}
  *value|=response[k+2]<<k;
 }
 begin(b);pin(b,OE,1);pin(b,CE,1);lt48_status end=send(b,NULL);
 return s==LT48_OK?end:s;
}
static lt48_status read_byte(bus*b,unsigned a,unsigned char*value){
 if(a>=8192)return LT48_INVALID_ARGUMENT;
 if(interrupted)return LT48_CANCELLED;
 if(byte_read)return read_whole_byte(b,a,value);
 *value=0;
 for(unsigned k=0;k<8;k++){
  make_read_bit(b,a,k);
  unsigned char bit=0;lt48_status s=send(b,&bit);if(s!=LT48_OK)return s;
  *value|=bit<<k;
 }
 return LT48_OK;
}
/* Hold CE/OE/WE from UDN sources during rail transitions. All FPGA pads
 * released then; DAC3 tracks DAC0 in 100mV requests, 1ms per step. This
 * removes the fixed FPGA-high back-power path during power-down. */
static lt48_status sources(bus*b,int on){unsigned char p[]={0x22,2*(CE+9)+on,0x22,2*(OE+9)+on,0x22,2*(WE+9)+on};return control(b,p,sizeof p);}
static lt48_status ramp(bus*b,int up){
 for(int v=up?1:50;up?v<=50:v>=0;v+=up?1:-1){
  unsigned char p[]={0x10,up?0:3,up?2*v:(v?v+20:0),0x10,up?3:0,up?(v+20):2*v};
  if(top_supply){p[1]=0;p[2]=0;p[4]=3;p[5]=v?v+20:0;}
  lt48_status s=control(b,p,sizeof p);if(s!=LT48_OK)return s;s=delay(b,1000);if(s!=LT48_OK)return s;
 }
 return LT48_OK;
}
static lt48_status power_on(bus*b){
 /* Direct serial writes do not call persistent relay-use counters. VCC38
  * is even-bank OUT29 (08); ground24 is odd-bank OUT27 (20). */
 unsigned char relays[]={0x53,0xaa,0x55,0x28,7,0x08,0,0,0,0x20,0,0,0,0x44,0x08,0,0,0,0x45,0x20,0,0,0};
 if(top_supply){relays[5]=0;relays[14]=0;}
 if(sink_ground){relays[9]=0;relays[19]=0;}
 lt48_status s=control(b,relays,sizeof relays);
 if(s==LT48_OK&&sink_ground){const unsigned char sink[]={0x1e,0x2f};s=control(b,sink,sizeof sink);}
 if(s==LT48_OK&&top_supply){const unsigned char src[]={0x22,0x4b};s=control(b,src,sizeof src);}if(s==LT48_OK)s=delay(b,10000);
 if(s==LT48_OK)s=sources(b,1);if(s==LT48_OK)s=ramp(b,1);
 if(s==LT48_OK)s=delay(b,20000);
 if(s==LT48_OK){begin(b);pin(b,CE,1);pin(b,OE,1);pin(b,WE,1);s=send(b,NULL);}
 if(s==LT48_OK)s=sources(b,0);
 if(s==LT48_OK&&!top_supply){const unsigned char p[]={0x10,3,0};s=control(b,p,sizeof p);}
 if(s==LT48_OK){s=lt48_runtime_transport_session_status(b->w,1);fprintf(stderr,"rail-status: %s\n",lt48_status_string(s));}
 if(s==LT48_OK)b->powered=1;return s;
}
static lt48_status reset_outputs(bus*b){
 lt48_protocol_transport api={wr,rd,NULL,NULL,LT48_EXECUTION_IDLE};
 lt48_runtime_transport_session_close(b->w);b->w=NULL;
 lt48_status s=lt48_runtime_transport_session_create(&api,b->p,&b->w);
 const unsigned char reset[]={0x49,0xaa,0x55};
 if(s==LT48_OK)s=control(b,reset,sizeof reset);return s;
}
static lt48_status power_off(bus*b){
 /* Keep controls inactive; address/data are driven LOW before VDD falls. */
 begin(b);pin(b,CE,1);pin(b,OE,1);pin(b,WE,1);
 for(unsigned i=0;i<13;i++)pin(b,ap[i],0);
 for(unsigned i=0;i<8;i++)pin(b,dp[i],0);
 lt48_status s=send(b,NULL);
 const unsigned char high[]={0x10,3,70};if(s==LT48_OK)s=control(b,high,sizeof high);
 if(s==LT48_OK)s=sources(b,1);
 if(s==LT48_OK){begin(b);pin(b,CE,2);pin(b,OE,2);pin(b,WE,2);s=send(b,NULL);}
 if(s==LT48_OK)s=delay(b,10000);
 if(s==LT48_OK)s=ramp(b,0);
 if(s==LT48_OK)s=delay(b,10000);
 /* Always attempt independent all-off; reset clears remaining source/relay
  * bits only after the successful ramp, or as emergency failure cleanup. */
 lt48_status clean=reset_outputs(b);
 b->powered=0;return s==LT48_OK?clean:s;
}
static lt48_status probe(bus*b){
 const unsigned addresses[]={0,1,0x7f,0x80,0x10f,0x110,0x1fff};
 const unsigned values[]={0,0xff,0x55,0xaa,0x0f,0xf0,0x1f,0x3f,0x7f};
 unsigned failed=0;lt48_status s=LT48_OK;
 for(unsigned a=0;s==LT48_OK&&a<sizeof addresses/sizeof*addresses;a++)
 for(unsigned v=0;s==LT48_OK&&v<sizeof values/sizeof*values;v++){
  unsigned char x=0,y=0;s=write_byte(b,addresses[a],values[v]);
  if(s==LT48_OK)s=read_byte(b,addresses[a],&x);
  if(s==LT48_OK)s=read_byte(b,addresses[a],&y);
  printf("{\"address\":%u,\"value\":%u,\"read1\":%u,\"read2\":%u,\"status\":%d}\n",addresses[a],values[v],x,y,s);fflush(stdout);
  if(x!=values[v]||y!=values[v])failed++;
 }
 fprintf(stderr,"powered checks: mismatches=%u status=%s\n",failed,lt48_status_string(s));
 if(s==LT48_OK)s=power_off(b);if(s==LT48_OK)s=delay(b,100000);if(s==LT48_OK)s=power_on(b);
 for(unsigned a=0;s==LT48_OK&&a<sizeof addresses/sizeof*addresses;a++){
  unsigned char x=0;s=read_byte(b,addresses[a],&x);printf("{\"power_cycle_address\":%u,\"expected\":127,\"read\":%u,\"status\":%d}\n",addresses[a],x,s);fflush(stdout);
  if(x!=127)failed++;
 }
 return s!=LT48_OK?s:failed?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status timing_test(bus*b){
 static const unsigned profiles[][3]={{0,0,0},{20,0,0},{0,20,0},{0,0,20},{20,20,20},{100,100,100}};
 static const unsigned addresses[]={0,1,0x7f,0x80,0x10f,0x110,0x1fff};
 static const unsigned values[]={0xff,0x7f,0xaa,0x55};
 unsigned total=0;lt48_status s=LT48_OK;
 for(unsigned round=0;s==LT48_OK&&round<3;round++)
 for(unsigned profile=0;s==LT48_OK&&profile<6;profile++)
 for(unsigned mode=0;s==LT48_OK&&mode<2;mode++){
  setup_us=profiles[profile][0];pulse_us=profiles[profile][1];hold_us=profiles[profile][2];ce_write=mode;unsigned bad=0;
  for(unsigned a=0;s==LT48_OK&&a<7;a++)for(unsigned v=0;s==LT48_OK&&v<4;v++){
   unsigned char x=0,y=0;
   /* Each target starts from zero to hold prior data constant. */
   s=write_byte(b,addresses[a],0);if(s==LT48_OK)s=read_byte(b,addresses[a],&x);
   if(s==LT48_OK&&x!=0)s=LT48_DEVICE_MISMATCH;
   if(s==LT48_OK)s=write_byte(b,addresses[a],values[v]);
   if(s==LT48_OK)s=read_byte(b,addresses[a],&x);if(s==LT48_OK)s=read_byte(b,addresses[a],&y);
   if(x!=values[v]||y!=values[v])bad++;
   printf("{\"round\":%u,\"profile\":%u,\"ce_write\":%u,\"address\":%u,\"value\":%u,\"read1\":%u,\"read2\":%u,\"status\":%d}\n",round,profile,mode,addresses[a],values[v],x,y,s);
  }
  total+=bad;fprintf(stderr,"timing round%u profile%u CE%u setup/pulse/hold=%u/%u/%u us: %u mismatches %s\n",round,profile,mode,setup_us,pulse_us,hold_us,bad,lt48_status_string(s));fflush(stderr);fflush(stdout);
 }
 setup_us=pulse_us=hold_us=0;ce_write=0;
 return s!=LT48_OK?s:total?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status memory_test(bus*b,const char*prefix,int clear_only){
 unsigned char expected[8192],actual[8192];unsigned failed=0;lt48_status s=LT48_OK;
 for(unsigned a=0;a<8192;a++)expected[a]=(unsigned char)(a^(a>>8));
 for(unsigned phase=clear_only?1:0;s==LT48_OK&&phase<2;phase++){
  if(phase)memset(expected,0xff,sizeof expected);
  for(unsigned a=0;s==LT48_OK&&a<8192;a++)s=write_byte(b,a,expected[a]);
  fprintf(stderr,"%s write: %s\n",phase?"FF":"address pattern",lt48_status_string(s));fflush(stderr);
  for(unsigned pass=0;s==LT48_OK&&pass<(clear_only?1u:3u);pass++){
   if(pass==2){s=power_off(b);if(s==LT48_OK)s=delay(b,100000);if(s==LT48_OK)s=power_on(b);}
   unsigned bad=0;
   for(unsigned a=0;s==LT48_OK&&a<8192;a++){
    s=read_byte(b,a,&actual[a]);if(s==LT48_OK&&actual[a]!=expected[a]){bad++;if(bad<=16)printf("{\"phase\":%u,\"pass\":%u,\"address\":%u,\"expected\":%u,\"read\":%u}\n",phase,pass,a,expected[a],actual[a]);}
   }
   if(s==LT48_OK){char path[4096];int n=snprintf(path,sizeof path,"%s-phase%u-read%u.bin",prefix,phase,pass);if(n<0||(size_t)n>=sizeof path)s=LT48_FILE_ERROR;else{FILE*f=fopen(path,"wbx");if(!f)s=LT48_FILE_ERROR;else{if(fwrite(actual,1,sizeof actual,f)!=sizeof actual)s=LT48_FILE_ERROR;if(fclose(f))s=LT48_FILE_ERROR;}}}
   printf("{\"phase\":%u,\"pass\":%u,\"mismatches\":%u,\"status\":%d}\n",phase,pass,bad,s);fflush(stdout);failed+=bad;
   fprintf(stderr,"phase%u read%u: %u mismatches %s\n",phase,pass,bad,lt48_status_string(s));fflush(stderr);
  }
 }
 return s!=LT48_OK?s:failed?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Invoke only after successful ramp-to-zero/reset: no active supply relay
 * or source. Ground every package pin including VDD, to discharge charge
 * storage and prevent I/O back-power during the off interval. */
static lt48_status ground_off_device(bus*b){
 const unsigned char gnd[]={0x53,0xaa,0x55,0x2c,3,0x20,0,0,0,0x45,0x20,0,0,0};
 lt48_status s=control(b,gnd,sizeof gnd);
 for(unsigned p=1;s==LT48_OK&&p<=28;p++){const unsigned char sink[]={0x1e,2*(p+9)+1};s=control(b,sink,sizeof sink);}
 if(s==LT48_OK)s=delay(b,10000);
 unsigned nonzero=0;
 for(unsigned p=1;s==LT48_OK&&p<=28;p++){unsigned char bit=0;begin(b);pin(b,p,2);pair(b,3,0);s=send(b,&bit);nonzero+=bit;}
 printf("{\"off_grounded_pins\":28,\"nonzero_logic_readbacks\":%u,\"status\":%d}\n",nonzero,s);fflush(stdout);
 return s!=LT48_OK?s:nonzero?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status retention_test(bus*b,const char*prefix){
 unsigned char expected[8192],actual[8192];lt48_status s=LT48_OK;unsigned failed=0;
 for(unsigned a=0;a<8192;a++)expected[a]=(unsigned char)(a^(a>>8));
 for(unsigned a=0;s==LT48_OK&&a<8192;a++)s=write_byte(b,a,expected[a]);
 for(unsigned pass=0;s==LT48_OK&&pass<3;pass++){
  if(pass==1){
   s=power_off(b);if(s==LT48_OK)s=ground_off_device(b);if(s!=LT48_OK)break;
   struct timespec start,end,remaining={30,0};
   if(clock_gettime(CLOCK_MONOTONIC,&start))return LT48_RUNTIME_ERROR;
   fprintf(stderr,"retention: outputs off, starting 30-second hold\n");fflush(stderr);
   while(nanosleep(&remaining,&remaining)&&errno==EINTR){if(interrupted)return LT48_CANCELLED;}
   if(interrupted)return LT48_CANCELLED;
   if(clock_gettime(CLOCK_MONOTONIC,&end))return LT48_RUNTIME_ERROR;
   double elapsed=end.tv_sec-start.tv_sec+(end.tv_nsec-start.tv_nsec)/1e9;
   printf("{\"outputs_off_seconds\":%.6f}\n",elapsed);fflush(stdout);
   if(elapsed<30.0)return LT48_RUNTIME_ERROR;
   fprintf(stderr,"retention: %.6f seconds off; restoring power\n",elapsed);fflush(stderr);
   s=reset_outputs(b);if(s==LT48_OK)s=power_on(b);
  }
  unsigned bad=0;
  for(unsigned a=0;s==LT48_OK&&a<8192;a++){
   s=read_byte(b,a,&actual[a]);if(s==LT48_OK&&actual[a]!=expected[a]){bad++;if(bad<=16)printf("{\"retention_pass\":%u,\"address\":%u,\"expected\":%u,\"read\":%u}\n",pass,a,expected[a],actual[a]);}
  }
  if(s==LT48_OK){char path[4096];int n=snprintf(path,sizeof path,"%s-retention-read%u.bin",prefix,pass);if(n<0||(size_t)n>=sizeof path)s=LT48_FILE_ERROR;else{FILE*f=fopen(path,"wbx");if(!f)s=LT48_FILE_ERROR;else{if(fwrite(actual,1,sizeof actual,f)!=sizeof actual)s=LT48_FILE_ERROR;if(fclose(f))s=LT48_FILE_ERROR;}}}
  printf("{\"retention_pass\":%u,\"mismatches\":%u,\"status\":%d}\n",pass,bad,s);fflush(stdout);
  fprintf(stderr,"retention read%u: %u mismatches %s\n",pass,bad,lt48_status_string(s));fflush(stderr);failed+=bad;
  /* Do not call a failed initial write/read a retention failure. */
  if(pass==0&&bad)return LT48_DEVICE_MISMATCH;
 }
 return s!=LT48_OK?s:failed?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Rewrite before every read-order comparison. No intervening read after
 * either initialization or the tested write; the first sample is explicit.
 * Both readers share the FPGA/pin mapping, so agreement is not independent
 * proof of stored contents. Four reads expose changes after first access. */
static lt48_status read_order_test(bus*b){
 const unsigned addresses[]={0,1,0x7f,0x80,0x10f,0x110,0x1fff};
 const unsigned values[]={0,0xff,0x7f,0xaa};
 unsigned failures=0;lt48_status s=LT48_OK;
 for(unsigned style=0;s==LT48_OK&&style<2;style++)
 for(unsigned a=0;s==LT48_OK&&a<7;a++)
 for(unsigned v=0;s==LT48_OK&&v<4;v++)
 for(unsigned first=0;s==LT48_OK&&first<2;first++){
  ce_write=style;
  s=write_byte(b,addresses[a],0);
  if(s==LT48_OK)s=write_byte(b,addresses[a],values[v]);
  unsigned char r[4]={0};unsigned completed=0;
  for(unsigned k=0;s==LT48_OK&&k<4;k++){
   byte_read=(first+k)%2;
   s=read_byte(b,addresses[a],&r[k]);
   if(s==LT48_OK){completed++;if(r[k]!=values[v])failures++;}
  }
  printf("{\"style\":%u,\"address\":%u,\"expected\":%u,\"first_byte_reader\":%u,\"reads\":[%u,%u,%u,%u],\"completed\":%u,\"status\":%d}\n",style,addresses[a],values[v],first,r[0],r[1],r[2],r[3],completed,s);fflush(stdout);
 }
 fprintf(stderr,"read-order mismatches=%u status=%s\n",failures,lt48_status_string(s));
 return s!=LT48_OK?s:failures?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Same powered session and prior zero state for each comparison. */
static lt48_status oe_trial(bus*b){
 const unsigned addresses[]={0,1,0x7f,0x80,0x10f,0x110,0x1fff};
 const unsigned values[]={0,0xff,0x55,0xaa,0x7f};
 unsigned bad[3]={0},total[3]={0};lt48_status s=LT48_OK;
 ce_write=0;
 for(unsigned round=0;s==LT48_OK&&round<2;round++)
 for(unsigned a=0;s==LT48_OK&&a<7;a++)
 for(unsigned v=0;s==LT48_OK&&v<5;v++)
 for(unsigned mode=0;s==LT48_OK&&mode<3;mode++){
  oe_write=mode;unsigned char zero=0,x=0,y=0;
  s=write_byte(b,addresses[a],0);
  if(s==LT48_OK)s=read_byte(b,addresses[a],&zero);
  if(s==LT48_OK)s=write_byte(b,addresses[a],values[v]);
  if(s==LT48_OK)s=read_byte(b,addresses[a],&x);
  if(s==LT48_OK)s=read_byte(b,addresses[a],&y);
  if(s==LT48_OK){total[mode]++;if(zero||x!=values[v]||y!=values[v])bad[mode]++;}
  printf("{\"round\":%u,\"mode\":%u,\"address\":%u,\"value\":%u,\"prezero\":%u,\"read1\":%u,\"read2\":%u,\"status\":%d}\n",round,mode,addresses[a],values[v],zero,x,y,s);fflush(stdout);
 }
 for(unsigned m=0;m<3;m++)fprintf(stderr,"OE trial mode%u: %u/%u mismatches\n",m,bad[m],total[m]);
 oe_write=0;
 return s!=LT48_OK?s:(bad[0]+bad[1]+bad[2])?LT48_DEVICE_MISMATCH:LT48_OK;
}
int main(int argc,char**argv){
 if((argc!=3&&argc!=4)||(strcmp(argv[2],"--probe")&&strcmp(argv[2],"--probe-ce")&&strcmp(argv[2],"--probe-top")&&strcmp(argv[2],"--memory")&&strcmp(argv[2],"--probe-sink")&&strcmp(argv[2],"--clear")&&strcmp(argv[2],"--retention")&&strcmp(argv[2],"--timing")&&strcmp(argv[2],"--read-order")&&strcmp(argv[2],"--oe-trial"))||((!strcmp(argv[2],"--memory")||!strcmp(argv[2],"--clear")||!strcmp(argv[2],"--retention"))&&argc!=4)){fprintf(stderr,"usage: %s PRIVATE_CACHE --probe|--probe-ce|--probe-top|--probe-sink|--memory PREFIX|--clear PREFIX|--retention PREFIX|--timing|--read-order|--oe-trial\n",argv[0]);return 2;}
 ce_write=!strcmp(argv[2],"--probe-ce");top_supply=!strcmp(argv[2],"--probe-top");sink_ground=!strcmp(argv[2],"--probe-sink");
 const char*read_mode=getenv("LT48_FRAM_READ");
 if(read_mode&&strcmp(read_mode,"byte")&&strcmp(read_mode,"bit")){fprintf(stderr,"LT48_FRAM_READ must be bit or byte\n");return 2;}
 byte_read=read_mode&&!strcmp(read_mode,"byte");
 fprintf(stderr,"read method: %s\n",byte_read?"one CE cycle, eight pin samples":"eight CE cycles");
 signal(SIGINT,stop);signal(SIGTERM,stop);
 lt48_context*c=NULL;lt48_programmer*p=NULL;lt48_programmer_info info[2];size_t n=0;lt48_transport_info ready={0};bus b={0};unsigned char*idle=NULL;size_t size=0;
 lt48_status s=lt48_load_verified_idle_fpga_internal(argv[1],&idle,&size);
 if(s==LT48_OK)s=lt48_context_create(&c);if(s==LT48_OK)s=lt48_programmer_list(c,info,2,&n);if(s==LT48_OK&&n!=1)s=LT48_SAFETY_REJECTED;
 if(s==LT48_OK)s=lt48_programmer_open(c,info[0].bus_number,info[0].device_address,&p);if(s==LT48_OK)s=lt48_programmer_transport_initialize(p,10000,&ready);
 b.p=p;lt48_protocol_transport api={wr,rd,NULL,NULL,LT48_EXECUTION_IDLE};if(s==LT48_OK)s=lt48_runtime_transport_session_create(&api,p,&b.w);
 const unsigned char reset[]={0x49,0xaa,0x55},load[]={0x38,0xaa,0x55},select[]={0x0f,15};
 if(s==LT48_OK)s=control(&b,reset,sizeof reset);if(s==LT48_OK)s=control(&b,load,sizeof load);if(s==LT48_OK)s=control(&b,idle,size);
 if(s==LT48_OK)s=control(&b,reset,sizeof reset);if(s==LT48_OK)s=control(&b,select,sizeof select);
 if(s==LT48_OK)s=power_on(&b);fprintf(stderr,"setup: %s\n",lt48_status_string(s));
 if(s==LT48_OK&&!strcmp(argv[2],"--oe-trial"))s=oe_trial(&b);
 else if(s==LT48_OK&&!strcmp(argv[2],"--read-order"))s=read_order_test(&b);
 else if(s==LT48_OK&&!strcmp(argv[2],"--timing"))s=timing_test(&b);
 else if(s==LT48_OK&&!strcmp(argv[2],"--retention"))s=retention_test(&b,argv[3]);
 else if(s==LT48_OK)s=(!strcmp(argv[2],"--memory")||!strcmp(argv[2],"--clear"))?memory_test(&b,argv[3],!strcmp(argv[2],"--clear")):probe(&b);
 lt48_status clean=LT48_OK;if(b.w){lt48_runtime_transport_session_close(b.w);b.w=NULL;clean=lt48_runtime_transport_session_create(&api,p,&b.w);if(clean==LT48_OK)clean=b.powered?power_off(&b):control(&b,reset,sizeof reset);}
 if(clean==LT48_OK&&b.w){const unsigned char led[]={0x40,s==LT48_OK?4:2};clean=control(&b,led,sizeof led);}
 fprintf(stderr,"operation: %s; cleanup: %s\n",lt48_status_string(s),lt48_status_string(clean));
 lt48_runtime_transport_session_close(b.w);lt48_programmer_close(p);lt48_context_destroy(c);free(idle);return s==LT48_OK&&clean==LT48_OK?0:1;
}
