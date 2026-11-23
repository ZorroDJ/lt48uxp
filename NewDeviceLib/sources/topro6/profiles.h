#include "configuration.h"
static const struct tp6_profile{unsigned kind,definition,family,pinmap,capacity,page,nominal,fields,configuration,regions,length[3],physical[3],logical[3],variables,operations,pins;}tp6_profiles[]={
{46,491,350,178,131072,0,400,1,25047,2,{65536,65536},{0,0},{0,65536},5,17,40},
{46,492,351,179,131072,0,400,1,25047,2,{65536,65536},{0,0},{0,65536},5,17,44},
{46,489,348,178,131072,0,330,1,25047,2,{65536,65536},{0,0},{0,65536},5,17,40},
{46,490,349,179,131072,0,330,1,25047,2,{65536,65536},{0,0},{0,65536},5,17,44},
};
static const struct{unsigned kind,definition;const char*vendor,*name;}tp6_members[]={
{46,491,"TOPRO","TP2808H"},
{46,492,"TOPRO","TP2808H *44"},
{46,492,"TOPRO","TP2808H *44Q"},
{46,489,"TOPRO","TP2808L"},
{46,490,"TOPRO","TP2808L *44"},
{46,490,"TOPRO","TP2808L *44Q"},
};
