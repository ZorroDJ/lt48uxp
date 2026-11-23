/* Exact source defaults on the FM91L03208 I adapters. No general fallback. */
static int mr7_default_context(unsigned d,unsigned op){return(d==0x3c||d==0x3d)&&(op==2||op==3||op==4);}
static int mr7_defaults(const lt48_compiled_wave*w,unsigned op){
static const unsigned a[][3]={{5,31,39},{5,55,39},{4,97,3},{5,102,3}};
static const unsigned b[][3]={{5,12,39},{5,28,39},{5,52,4}};
static const unsigned c[][3]={{4,22,3}};
const unsigned(*e)[3]=op==2?a:op==3?b:c;size_t n=op==2?4:op==3?3:1;
if(lt48_compiled_wave_defaulted_pin_count(w)!=n)return 0;
for(size_t i=0;i<n;i++){lt48_defaulted_logical_pin p;if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK||p.logical_pin!=e[i][0]||p.compact_offset!=e[i][1]||p.compact_opcode!=e[i][2]||p.classification!=LT48_DEFAULTED_PIN_UNRESOLVED)return 0;}return 1;}
