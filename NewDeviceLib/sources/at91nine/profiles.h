struct a91_profile{unsigned cls,d,m,p,cap,vcc,core,page,width,locks;const char*source,*sha;};
static const struct a91_profile a91_profiles[]={
{2033,195,82,75,131072,0,150,256,2,255,"GDABA__.rez","77e939e536a8af7d76fa8d661927e7f1e712eb7b1441fe6e7c86ddbeefdb51ec"},
{2038,301,182,115,131072,0,165,256,1,255,"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36"},
{2033,196,82,75,262144,0,150,256,2,65535,"GDABA__.rez","77e939e536a8af7d76fa8d661927e7f1e712eb7b1441fe6e7c86ddbeefdb51ec"},
{2038,302,182,115,262144,0,165,256,1,65535,"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36"},
{2038,299,180,114,32768,2,165,128,1,255,"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36"},
{2033,194,81,75,65536,0,150,128,2,65535,"GDABA__.rez","77e939e536a8af7d76fa8d661927e7f1e712eb7b1441fe6e7c86ddbeefdb51ec"},
{2038,300,181,115,65536,0,165,128,1,65535,"GDAE___.rez","9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36"},
};
static const struct{unsigned cls,d;const char*vendor,*name;}a91_names[]={
{2033,195,"Atmel","AT91SAM7S128 @64TQ"},
{2038,301,"Atmel","AT91SAM7S128B @64TQ"},
{2033,196,"Atmel","AT91SAM7S256 @64TQ"},
{2038,302,"Atmel","AT91SAM7S256B @64TQ"},
{2038,299,"Atmel","AT91SAM7S32 *48LQ"},
{2038,299,"Atmel","AT91SAM7S32B *48LQ"},
{2033,194,"Atmel","AT91SAM7S64 @64TQ"},
{2038,300,"Atmel","AT91SAM7S64B @64TQ"},
{2038,300,"iPanel","IPNL700 @64TQ"},
};
