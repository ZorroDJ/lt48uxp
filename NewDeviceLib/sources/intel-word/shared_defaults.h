/* Source-exact Sharp pin defaults. See Intel/Sharp shared-word audit. */
#ifndef LT48_INTEL_SHARED_DEFAULTS_H
#define LT48_INTEL_SHARED_DEFAULTS_H
static int intel_shared_default_context(unsigned d,unsigned op){switch(d){
case 0x18:return op==2 || op==4;
case 0x19:return op==2 || op==4;
case 0x1a:return op==2 || op==4;
case 0x82:return op==2;
case 0x83:return op==2;
case 0x86:return op==2;
case 0x87:return op==2;
default:return 0;}}
static int intel_shared_defaults(const lt48_compiled_wave *w,unsigned d,unsigned op){
struct expected {unsigned pin,at,code,kind;};const struct expected *e=NULL;size_t count=0;
if(d==0x86 && op==2){static const struct expected a[]={{7,40,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{0,43,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,46,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{5,54,3,LT48_DEFAULTED_PIN_UNRESOLVED}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x18 && op==2){static const struct expected a[]={{7,77,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,89,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{4,97,3,LT48_DEFAULTED_PIN_UNRESOLVED},{3,112,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x18 && op==4){static const struct expected a[]={{7,2,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,14,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{4,22,3,LT48_DEFAULTED_PIN_UNRESOLVED},{3,27,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x87 && op==2){static const struct expected a[]={{7,40,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{0,43,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,46,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{5,54,3,LT48_DEFAULTED_PIN_UNRESOLVED}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x1a && op==2){static const struct expected a[]={{7,77,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,89,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{4,97,3,LT48_DEFAULTED_PIN_UNRESOLVED},{3,112,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x1a && op==4){static const struct expected a[]={{7,2,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,14,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{4,22,3,LT48_DEFAULTED_PIN_UNRESOLVED},{3,27,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x19 && op==2){static const struct expected a[]={{7,77,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,89,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{4,97,3,LT48_DEFAULTED_PIN_UNRESOLVED},{3,112,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x19 && op==4){static const struct expected a[]={{7,2,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{3,14,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{4,22,3,LT48_DEFAULTED_PIN_UNRESOLVED},{3,27,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x82 && op==2){static const struct expected a[]={{0,43,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{8,49,3,LT48_DEFAULTED_PIN_UNRESOLVED}};e=a;count=sizeof(a)/sizeof(*a);}
if(d==0x83 && op==2){static const struct expected a[]={{7,40,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{0,43,17,LT48_DEFAULTED_PIN_RESIDENT_NOOP},{8,49,3,LT48_DEFAULTED_PIN_UNRESOLVED}};e=a;count=sizeof(a)/sizeof(*a);}
if(!e || lt48_compiled_wave_defaulted_pin_count(w)!=count)return 0;
for(size_t i=0;i<count;i++){lt48_defaulted_logical_pin p;if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK || p.logical_pin!=e[i].pin || p.compact_offset!=e[i].at || p.compact_opcode!=e[i].code || p.classification!=e[i].kind)return 0;}return 1;}
#endif
