#ifndef LT48_HC11_INTERNAL_H
#define LT48_HC11_INTERNAL_H
#include <labtool48/labtool48.h>
lt48_status lt48_hc11_open_candidate(const lt48_installation *,size_t,lt48_hc11_resources **);
int lt48_hc11_is_extension(const lt48_hc11_resources *);
#endif
