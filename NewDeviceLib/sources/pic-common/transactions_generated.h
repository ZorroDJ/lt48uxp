/* Generated from independently replayed compact host plans; no admission. */
#ifndef LT48_PIC_COMMON_TRANSACTIONS_GENERATED_H
#define LT48_PIC_COMMON_TRANSACTIONS_GENERATED_H
#include "transaction.h"
static const lt48_pic_transaction_plan pic_common_transactions[] = {
 {7,{3,30},{9,30},{4,10}},
 {8,{3,10},{10,10},{4,10}},
};
static inline const lt48_pic_transaction_plan *lt48_pic_transaction_find(unsigned mode) {
 for(size_t i=0;i<sizeof(pic_common_transactions)/sizeof(*pic_common_transactions);i++)
  if(pic_common_transactions[i].mode==mode)return &pic_common_transactions[i];
 return NULL;
}
#endif
