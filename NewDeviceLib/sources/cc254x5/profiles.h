/* Original capacities and exact catalogue aliases; source payloads BYO. */
static const struct c25_profile{unsigned definition,family,pinmap,capacity,variables,ops,apps,nominal;}c25_profiles[]={
{630,475,232,131072,4,19,6,320},
{631,476,232,262144,4,19,6,320},
};
static const struct{unsigned definition,manufacturer,device;const char*vendor,*name;}c25_members[]={
{630,141,0,"TI","CC2540F128 *40QFN"},
{631,141,0,"TI","CC2540F256 *40QFN"},
{631,141,0,"TI","CC2540TF256 *40QFN"},
{630,65,0,"TI","CC2541F128 *40QFN"},
{631,65,0,"TI","CC2541F256 *40QFN"},
};
