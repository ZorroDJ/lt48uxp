#include "../novatek-aux12/regions.h"
static int nt25_admitted(unsigned d){return(d>=0x1dd&&d<=0x1e8)||ntaux_admitted(d);}
