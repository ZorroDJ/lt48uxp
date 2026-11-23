/* Authored descriptor repair. The original immutable F2e iterator has73
 * values, while D2a/D2b declare72. C and GUI retain all73 source cycles with
 * bounded storage, exposing the value the Windows converter kept in padding. */
#ifndef LT48_EJED9_GEOMETRY_H
#define LT48_EJED9_GEOMETRY_H
static inline unsigned ejed9_aux_count(unsigned d,unsigned m,unsigned p,unsigned region,
 unsigned id,unsigned declared,unsigned actual){
 if(((d==42u&&m==38u)||(d==43u&&m==39u))&&p==15u&&!region&&id==46u&&declared==72u&&actual==73u)return 73u;
 return declared;
}
#endif
