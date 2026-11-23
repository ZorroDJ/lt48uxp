#include "configuration.h"
static const struct nv8_profile{unsigned definition,family,pinmap,capacity,fields,configuration,regions,length[3],physical[3],logical[3],variables,ops,apps,nominal,lock;const struct numicro_field*schema;}nv8_profiles[]={
{456,319,153,10240,9,25290,3,{4096,4096,2048},{0,12288,65536},{0,12288,16384},3,16,4,500,0,nv8_fields_25290},
{469,328,160,73728,9,25445,3,{65536,4096,4096},{0,126976,1048576},{0,126976,1048576},2,19,6,500,0,nv8_fields_25445},
{467,326,158,73728,13,25385,3,{65536,4096,4096},{0,126976,1048576},{0,126976,1048576},2,19,6,500,1,nv8_fields_25385},
{468,327,159,73728,9,25445,3,{65536,4096,4096},{0,126976,1048576},{0,126976,1048576},2,19,6,500,0,nv8_fields_25445},
{465,325,157,135168,14,25408,2,{131072,4096},{0,1048576},{0,1048576},2,16,4,320,1,nv8_fields_25408},
{466,325,157,266240,14,25408,2,{262144,4096},{0,1048576},{0,1048576},2,16,4,320,1,nv8_fields_25408},
};
static const struct{unsigned definition,manufacturer,device;const char*vendor,*name;}nv8_members[]={
{456,0,0,"Nuvoton","N79E8432A *16SO"},
{469,0,0,"Nuvoton","NUC029LAN *48LQ"},
{467,0,0,"Nuvoton","NUC029LDE *48LQ"},
{468,0,0,"Nuvoton","NUC029ZAN @32QFN"},
{468,0,0,"Nuvoton","NUC029ZPOE @32QFN"},
{465,0,0,"Nuvoton","NUC126LE4AE @48LQ"},
{466,0,0,"Nuvoton","NUC126LG4AE @48LQ"},
{465,0,0,"Nuvoton","NUC126NE4AE @48QFN"},
};
