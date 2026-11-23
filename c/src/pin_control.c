#include <labtool48/pin_control.h>
#include <stdio.h>
#include <string.h>
static const unsigned char ground[]={15,24,34};
static const unsigned char supply[]={25,34,36,38,40,44,46};
/* Resident ROM0781, ordered by ZIF-1. */
static const unsigned char sink_reg[]={3,4,2,5,2,5,3,4,6,1,7,0,4,3,5,2,7,0,6,1,4,5,6,7,3,2,1,0,4,3,5,2,1,6,0,7,0,7,1,6,2,5,3,4,6,1,7,0};
static const unsigned char sink_mask[]={2,2,2,2,1,1,1,1,1,1,1,1,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,16,16,16,16,16,16,16,16,32,32,32,32,32,32,32,32,2,2,2,2};
static int position(const unsigned char *p,size_t n,unsigned pin){for(size_t i=0;i<n;i++)if(p[i]==pin)return (int)i;return -1;}
lt48_status lt48_pin_control_capability(unsigned pin, lt48_pin_capability *out) {
 if (!out || pin < 1 || pin > 48) return LT48_INVALID_ARGUMENT;
 lt48_pin_capability p = {0};
 int vcc = position(supply, 7, pin), gnd = position(ground, 3, pin);
 p.available_routes = LT48_PIN_FPGA_LOW | LT48_PIN_FPGA_HIGH |
     LT48_PIN_BOTTOM_SOURCE | LT48_PIN_TOP_SOURCE | LT48_PIN_TPIC_SINK;
 p.bottom_bank = pin % 2 ? 2 : 1;
 p.bottom_output = (pin + 1) / 2;
 p.bottom_dac_channel = p.bottom_bank == 1 ? 1 : 2;
 if (vcc >= 0) {p.available_routes |= LT48_PIN_VCC_RELAY; p.vcc_relay_output = 26 + (unsigned)vcc;}
 if (gnd >= 0) {p.available_routes |= LT48_PIN_GND_RELAY; p.ground_relay_output = 26 + (unsigned)gnd;}
 p.sink_xdata_address = (unsigned short)(0xf830 + sink_reg[pin - 1]);
 p.sink_mask = sink_mask[pin - 1];
 p.top_source_enable_operand = (unsigned char)(2 * (pin - 1) + 1);
 *out = p;
 return LT48_OK;
}
static lt48_status bad(char*r,size_t n,const char*s){if(r&&n)snprintf(r,n,"%s",s);return LT48_SAFETY_REJECTED;}
static void command(lt48_pin_plan*p,const unsigned char*b,size_t n){lt48_pin_step*s=&p->steps[p->count++];s->kind=LT48_PIN_CONTROLLER_BYTES;s->size=n;memcpy(s->bytes,b,n);}
static void wait_step(lt48_pin_plan*p,unsigned us){lt48_pin_step*s=&p->steps[p->count++];s->kind=LT48_PIN_WAIT_US;s->delay_us=us;}
static void status_step(lt48_pin_plan*p){p->steps[p->count++].kind=LT48_PIN_STATUS_CHECK;}
lt48_status lt48_pin_control_plan(const lt48_pin_configuration*c,lt48_pin_plan*out,char*why,size_t n){
 lt48_pin_plan p={0};unsigned used=0;int active=0,relay=0;
 if(!c||!out)return LT48_INVALID_ARGUMENT;
 if(!c->verified_baseline_fpga)return bad(why,n,"Verified baseline FPGA is required for reset and digital states");
 for(unsigned i=0;i<48;i++){
  unsigned r=c->routes[i];int idx;
  if(r&~127u)return bad(why,n,"Unknown pin route");
  if(r&&(r&(r-1)))return bad(why,n,"Multiple drive paths on one ZIF contact");
  if(r)active=1;
  if(r==LT48_PIN_VCC_RELAY){idx=position(supply,7,i+1);if(idx<0)return bad(why,n,"No VCC relay at requested ZIF contact");p.serial[0][0]|=(unsigned char)(0x40u>>idx);used|=1;relay=1;}
  if(r==LT48_PIN_GND_RELAY){idx=position(ground,3,i+1);if(idx<0)return bad(why,n,"No ground relay at requested ZIF contact");p.serial[1][0]|=(unsigned char)(0x40u>>idx);relay=1;}
  if(r==LT48_PIN_BOTTOM_SOURCE){unsigned bank=(i&1)?0:1,ord=i/2;p.serial[bank][3-ord/8]|=(unsigned char)(0x80u>>(ord%8));used|=1u<<(bank?2:1);}
  if(r==LT48_PIN_TOP_SOURCE)used|=8;
  if(r==LT48_PIN_TPIC_SINK)p.sink_registers[sink_reg[i]]|=sink_mask[i];
 }
 if(active&&(!c->discharge_us||!c->settle_us))return bad(why,n,"Explicit discharge and settle intervals required");
 for(unsigned ch=0;ch<4;ch++){
  unsigned v=c->millivolts[ch],scale=ch?100:50,offset=ch?(ch==3?20:13):0;
  if(v&&!(used&(1u<<ch)))return bad(why,n,"Energized rail has no selected source route");
  if(v&&(!c->maximum_millivolts[ch]||v>c->maximum_millivolts[ch]))return bad(why,n,"Rail exceeds declared device/adapter ceiling");
  if(v%scale)return bad(why,n,"Voltage not representable at DAC resolution");
  if(v&&(v/scale>255-offset))return bad(why,n,"Voltage exceeds DAC code range");
  p.dac_codes[ch]=(unsigned char)(v?v/scale+offset:0);
 }
 {const unsigned char b[]={0x49,0xaa,0x55};command(&p,b,sizeof b);}
 if(c->discharge_us)wait_step(&p,c->discharge_us);
 status_step(&p);
 /* Update shadows and hardware together. Clear all auxiliary and source bits
  * not explicitly requested, including the unqualified global bank2 flag. */
 {unsigned char b[13]={0x53,0xaa,0x55,0x28,7};memcpy(b+5,p.serial,8);command(&p,b,sizeof b);}
 for(unsigned bank=0;bank<2;bank++){unsigned char b[5]={(unsigned char)(0x44+bank)};memcpy(b+1,p.serial[bank],4);command(&p,b,5);}
 for(unsigned i=0;i<48;i++){
  unsigned r=c->routes[i];
  if(r==LT48_PIN_TPIC_SINK||r==LT48_PIN_TOP_SOURCE){unsigned char b[2]={(unsigned char)(r==LT48_PIN_TPIC_SINK?0x1e:0x22),(unsigned char)(2*i+1)};command(&p,b,2);}
 }
 if(relay)wait_step(&p,c->settle_us);
 for(unsigned ch=0;ch<4;ch++)if(p.dac_codes[ch]){unsigned char b[3]={0x10,(unsigned char)ch,p.dac_codes[ch]};command(&p,b,3);wait_step(&p,c->settle_us);status_step(&p);}
 for(unsigned i=0;i<48;i++)if(c->routes[i]==LT48_PIN_FPGA_LOW||c->routes[i]==LT48_PIN_FPGA_HIGH){lt48_pin_step*s=&p.steps[p.count++];s->kind=LT48_PIN_FPGA_BYTE;s->size=1;s->bytes[0]=(unsigned char)(4*i+(c->routes[i]==LT48_PIN_FPGA_HIGH));}
 status_step(&p);*out=p;if(why&&n)why[0]=0;return LT48_OK;
}
