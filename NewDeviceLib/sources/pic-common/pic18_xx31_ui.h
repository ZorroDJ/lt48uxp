#ifndef LT48_PIC18_XX31_UI_H
#define LT48_PIC18_XX31_UI_H
#include <labtool48/labtool48.h>
/* Exact audited XX31 source bindings; admission remains a separate gate. */
static inline int lt48_pic18_xx31_configuration_binding(uint32_t d,uint16_t f,uint16_t p) {
 if(d==104u&&f==91u&&p==39u)return 1;
 if(d==108u&&f==95u&&p==39u)return 1;
 if(d==105u&&f==92u&&p==40u)return 1;
 if(d==106u&&f==93u&&p==41u)return 1;
 if(d==107u&&f==94u&&p==42u)return 1;
 if(d==109u&&f==96u&&p==40u)return 1;
 if(d==110u&&f==97u&&p==41u)return 1;
 if(d==111u&&f==98u&&p==42u)return 1;
 return 0;
}
#endif
