#include "admission.h"
struct r8c8_profile{unsigned cls,d,m,p,pins,cap,variables,operations,xp,regions;unsigned lengths[2],physical[2];const char*source,*sha,*name;};
static const struct r8c8_profile r8c8_profiles[]={
{2038,330,205,132,44,16384,3,12,1,1,{16384,0},{49152,0},"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36","R5F21244SNFP @52TQ"},
{2038,331,206,132,44,32768,4,12,1,1,{32768,0},{32768,0},"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36","R5F21246SNFP @52TQ"},
{2048,615,461,224,44,65536,2,16,0,2,{2048,16384},{9216,49152},"GDCC___.rez","c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640","R5F21254SNFP @52TQ"},
{2048,616,462,224,44,65536,3,16,0,2,{2048,32768},{9216,32768},"GDCC___.rez","c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640","R5F21256SNFP @52TQ"},
{2038,328,203,131,32,8192,3,12,1,1,{8192,0},{57344,0},"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36","R5F21262SNFP *32TQ"},
{2038,329,204,131,32,16384,3,12,1,1,{16384,0},{49152,0},"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36","R5F21264SNFP *32TQ"},
{2048,613,459,223,32,65536,2,16,0,2,{2048,8192},{9216,57344},"GDCC___.rez","c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640","R5F21272SNFP *32TQ"},
{2048,614,460,223,32,65536,2,16,0,2,{2048,16384},{9216,49152},"GDCC___.rez","c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640","R5F21274SNFP *32TQ"}};
static inline const struct r8c8_profile*r8c8_find(unsigned cls,unsigned d){for(size_t i=0;i<sizeof r8c8_profiles/sizeof*r8c8_profiles;i++)if(r8c8_profiles[i].cls==cls&&r8c8_profiles[i].d==d)return r8c8_profiles+i;return NULL;}
