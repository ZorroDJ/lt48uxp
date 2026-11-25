#define _POSIX_C_SOURCE 200809L
#include "at89_s8253.h"
#include "st7.h"
#include "tms370.h"
#include "tms87.h"
#include "../../NewDeviceLib/sources/fm8one/geometry.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_xx2_ui.h"
#include "../../NewDeviceLib/sources/legacy-nor/ams_nor_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/fujitsu_002_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/sharp_s3_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/atmel_lv2048b_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/w49_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/actrans4_admission.h"
#include "../../NewDeviceLib/sources/efst1/member.h"
#include "../../NewDeviceLib/sources/sm2964two/member.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_lv040_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/nexflash_010_admission.h"
#ifdef LT48_ADMISSION_TRIAL_HEADER
#include LT48_ADMISSION_TRIAL_HEADER
#else
#include "../../NewDeviceLib/sources/legacy-nor/batch_admissions.h"
#endif
#include "../../NewDeviceLib/sources/legacy-nor/macronix_psop3_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/m29f800_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/m28f800_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/mx26b_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l8100_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l8000_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_l3211_admission.h"
#include "../../NewDeviceLib/sources/legacy-nor/macronix_f1601_admission.h"

#include "../../NewDeviceLib/sources/pic-common/adapter_guidance.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_x220_adapter.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_xx31_adapter.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_f6x20_adapter.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_f6_adapter.h"
#include "../../NewDeviceLib/sources/pic-common/baseline12_adapter_guidance.h"
#include "../../NewDeviceLib/sources/pic-common/package_label_corrections.h"
#include <labtool48/labtool48.h>
#include <labtool48/configuration_profiles_generated.h>
#include <labtool48/families_generated.h>
#include "internal.h"
#include "../../NewDeviceLib/sources/x88one/member.h"
#include "../../NewDeviceLib/sources/pss2005one/member.h"
#include "../../NewDeviceLib/sources/w27e4096two/member.h"
#include "../../NewDeviceLib/sources/mxwordtwo/member.h"
#include "../../NewDeviceLib/sources/mx27pageone/member.h"
#include "../../NewDeviceLib/sources/w29wordtwo/member.h"

#include "../../NewDeviceLib/sources/memreuse7/members.h"
#include "../../NewDeviceLib/sources/gal59/profiles.h"
#include "../../NewDeviceLib/sources/cpld69/profiles.h"
#include "../../NewDeviceLib/sources/palce54/profiles.h"
#include "../../NewDeviceLib/sources/palce8/profiles.h"
#include "../../NewDeviceLib/sources/mach11/profiles.h"
#include "../../NewDeviceLib/sources/ejed9/profiles.h"
#include "../../NewDeviceLib/sources/epm44/profiles.h"
#include "hc908.h"
#include "hc705_bootstrap.h"
#include "hc705_jp.h"
#include "hc705_ca.h"
#include "hc705_bd.h"
#include "hc705_bx.h"
#include "hc705_jjsrpl.h"
#include "s08.h"
#include "rs08.h"
#include "hc11.h"
#include "hc711.h"
#include "../../NewDeviceLib/sources/pic-common/known_combination_carriers.h"
#include "pic18_j.h"
#include "../../NewDeviceLib/sources/mx28f1000/admission.h"

#include "../../NewDeviceLib/sources/legacy-st28/admission.h"
#include "../../NewDeviceLib/sources/legacy-sst1/admission.h"
#include "../../NewDeviceLib/sources/legacy-mx28p2/admission.h"
#include "../../NewDeviceLib/sources/legacy-alliance2/admission.h"
#include "../../NewDeviceLib/sources/legacy-macronix-bootblock4/admission.h"
#include "../../NewDeviceLib/sources/addtab-remaining73/admission.h"
#include "addtab_dl46_definitions.h"

#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/st2009two/geometry.h"
#include "../../NewDeviceLib/sources/st2009two/member.h"
#include "../../NewDeviceLib/sources/w78e354one/member.h"
#include "../../NewDeviceLib/sources/w78e354one/admission.h"
#include "../../NewDeviceLib/sources/pal46/profiles.h"
#include "../../NewDeviceLib/sources/pal29/profiles.h"
#include "../../NewDeviceLib/sources/ep35/profiles.h"
#include "../../NewDeviceLib/sources/pal52/profiles.h"
#include "../../NewDeviceLib/sources/peel46/profiles.h"
#include "../../NewDeviceLib/sources/pz20/profiles.h"
#include "../../NewDeviceLib/sources/compact20/profiles.h"
#include "../../NewDeviceLib/sources/pls26/profiles.h"
#include "../../NewDeviceLib/sources/epx6/profiles.h"
#include "../../NewDeviceLib/sources/legacy51-9/profiles.h"
#include "../../NewDeviceLib/sources/myson2/profiles.h"
#include "../../NewDeviceLib/sources/myson44/profiles.h"
#include "../../NewDeviceLib/sources/mysonM2/profiles.h"
#include <sys/stat.h>
#include "../../NewDeviceLib/sources/pic-common/pic18_admissions_generated.h"

/* Complete XP word-flash batch; source audit in XP_WORD138_BATCH_AUDIT.md. */
#include "generated/xp_shared_word_batch.inc"
#include "../../NewDeviceLib/sources/lv800three/members.h"
#include "generated/xp_atmel_sanyo15_batch.inc"
#include "generated/intel_shared_word118_batch.inc"
static int xp_amic24_batch(unsigned d){return d==0x99||d==0x9a||d==0xa1||d==0xa2;}
static int xp_word63_batch(unsigned definition) {
    static const unsigned defs[]={0,1,0x69,0x6a,0x6d,0x6e,0x71,0x72,0x75,0x76,0x79,0x7a,0x7d,0x7e,0x81,0x82,0x85,0x86,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x95,0x96,0x97,0x98,0x9b,0x9c,0x9d,0x9e,0x9f,0xa3,0xa4,0xa5,0xa6};
    for(size_t i=0;i<sizeof(defs)/sizeof(*defs);i++)if(definition==defs[i])return 1;
    return 0;
}
static int xp_word138_batch(unsigned definition) {
    switch (definition) {
    case 0xe:
    case 0xf:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x5e:
    case 0x5f:
    case 0xc3:
    case 0xc4:
    case 0x110:
    case 0x111:
    case 0x112:
    case 0x113:
    case 0x114:
    case 0x115:
    case 0x116:
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11a:
    case 0x11b:
    case 0x11c:
    case 0x11d:
    case 0x11e:
    case 0x11f:
    case 0x120:
    case 0x121:
    case 0x122:
    case 0x123:
    case 0x124:
    case 0x125:
    case 0x126:
    case 0x127:
    case 0x128:
    case 0x129:
    case 0x12a:
    case 0x12b:
    case 0x12c:
    case 0x12d:
    case 0x12e:
    case 0x12f:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x137:
    case 0x138:
    case 0x139:
    case 0x13a:
    case 0x13b:
    case 0x13c:
    case 0x13d:
    case 0x13e:
    case 0x13f:
    case 0x140:
    case 0x141:
    case 0x142:
    case 0x143:
    case 0x144:
    case 0x145:
    case 0x146:
    case 0x147:
    case 0x148:
        return 1;
    default: return 0;
    }
}

/* Exact source definitions audited in XP_SO44_NOR_BATCH_AUDIT.md. */
static int intel_shared_byte58_batch(unsigned d){switch(d){case 0x3d:case 0x3e:case 0x3f:case 0x40:case 0x41:case 0x42:case 0x43:case 0x44:case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4a:case 0x4b:case 0x4c:case 0x4d:case 0x4e:case 0x4f:case 0x50:case 0x51:case 0x52:case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:case 0x58:case 0x59:case 0x5a:case 0x5b:case 0x5c:case 0x5d:case 0x5e:case 0x60:case 0x61:return 1;default:return 0;}}
static int xp_shared_byte75_batch(unsigned d){switch(d){case 0x4:case 0x5:case 0x6:case 0xe:case 0xf:case 0x10:case 0x11:case 0x12:case 0x3d:case 0x3e:case 0x40:case 0x41:case 0x42:case 0x43:case 0x44:case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4a:case 0x4b:case 0x50:case 0x51:case 0x52:case 0x53:case 0x5b:case 0x5c:case 0x5d:case 0x6a:case 0x6b:case 0x6c:case 0x75:case 0x76:case 0x7b:case 0x7c:case 0x7d:case 0x83:case 0x84:case 0xb4:case 0xb6:case 0xb7:case 0xc2:return 1;default:return 0;}}
static int xp_byte60_batch(unsigned d) {switch(d){case 0x23:case 0x24:case 0x4c:case 0x56:case 0x57:case 0x58:case 0x59:case 0x5a:case 0x5e:case 0xa7:case 0xa9:case 0xab:case 0xad:case 0xb8:case 0xb9:case 0xba:case 0xbb:return 1;default:return 0;}}
static int xp_byte128_batch(unsigned d) { switch(d) {case 0:case 1:case 2:case 3:case 19:case 20:case 21:case 22:case 25:case 26:case 27:case 28:case 31:case 32:case 33:case 34:case 37:case 38:case 41:case 42:case 43:case 44:case 46:case 47:case 48:case 49:case 53:case 54:case 55:case 56:case 57:case 58:case 59:case 60:case 63:case 84:case 85:case 95:case 96:case 97:case 98:case 99:case 101:case 103:case 110:case 112:case 119:case 121:case 126:case 128:case 178:case 179:case 181:case 188:return 1; default:return 0;} }
static int xp_sanyo4_batch(unsigned d) {return d>=0xa6 && d<=0xa9;}
static int xp_seiko12_batch(unsigned d) {return d>=0x24 && d<=0x27;}
static int xp_intel29_batch(unsigned d) {return d==0x10 || d==0x11 || (d>=0x37 && d<=0x3b);}
static int intel_word100_batch(unsigned d) { switch(d) {case 0x8:case 0x9:case 0xa:case 0xb:case 0xc:case 0xd:case 0x3e:case 0x3f:case 0x45:case 0x4a:case 0x4b:case 0x4c:case 0x4d:case 0x4e:case 0x4f:case 0x56:case 0x57:case 0x58:case 0x59:case 0x5a:case 0x5b:case 0x5c:case 0x5d:case 0x5e:case 0x5f:case 0x60:case 0x61:case 0x62:case 0x63:case 0x64:case 0x65:case 0x66:case 0x67:case 0x68:case 0x69:case 0x76:case 0x77:case 0x78:case 0x79:case 0x7a:case 0x7b:case 0x8e:case 0x8f:case 0x90:case 0x91:case 0x92:case 0x93:case 0xaa:case 0xab:case 0xac:case 0xad:case 0xae:case 0xaf:return 1; default:return 0;} }
static int xp_so44_batch(unsigned definition) {
    switch (definition) {
    case 0x1f: case 0x20: case 0x23: case 0x24: case 0x27: case 0x28:
    case 0x2b: case 0x2c: case 0x31: case 0x32:
    case 0x6b: case 0x6c: case 0x6f: case 0x70: case 0x73: case 0x74:
    case 0x7b: case 0x7c: case 0x7f: case 0x80: case 0x83: case 0x84:
    case 0x87: case 0x88: return 1;
    default: return 0;
    }
}

/* Exact AddTab batch definitions; see ADDTAB_WORD131_BATCH_AUDIT.md. */
static int addtab_dl54_batch(unsigned definition) {
    static const unsigned defs[]={0x8,0xb,0x15,0x16,0x17,0x18,0x19,0x1a,0x24,0x44,0x45,0x46,0x47,0x58,0x59,0x5a,0x5b,0x62,0x63,0x65};
    for(size_t i=0;i<sizeof(defs)/sizeof(*defs);i++)if(definition==defs[i])return 1;
    return 0;
}
static int addtab_gl67_batch(unsigned definition) {
    static const unsigned defs[]={0x48,0x4d,0x56,0x5e,0x68,0x6a,0x6b,0x6c,0x9b,0xa2,0xab,0xd5,0xd7,0xd9,0xdb,0x136,0x139,0x13a};
    for(size_t i=0;i<sizeof(defs)/sizeof(*defs);i++)if(definition==defs[i])return 1;
    return 0;
}
static int addtab_glp34_batch(unsigned d) {return d==0xb7||d==0xcc||d==0xcd||d==0xce;}
static int addtab_gln86_batch(unsigned d) {switch(d){case 0xba:case 0xbb:case 0xbc:case 0xc1:case 0xc3:case 0xc4:case 0xc5:case 0xc8:case 0xc9:case 0xca:case 0xdf:case 0xe0:case 0xe1:return 1;default:return 0;}}
static int addtab_word47_batch(unsigned d) {return d>=0x83 && d<=0x8c;}
static int addtab_gln20_batch(unsigned d) {return d==0xb8||d==0xb9||d==0xbf||d==0xc0||d==0xdd||d==0xde;}
static int addtab_glm22_batch(unsigned d) {return (d>=0x49 && d<=0x4c)||(d>=0x9c && d<=0x9f);}
static int addtab_gl47_batch(unsigned d) {switch(d){case 0x135:case 0x137:case 0x138:case 0x4e:case 0x4f:case 0x50:case 0x51:case 0x52:case 0x53:case 0x54:case 0x66:case 0x67:case 0xa3:case 0xa4:case 0xa5:case 0xa6:case 0xa7:case 0xa8:case 0xa9:case 0xd2:case 0xd3:return 1;default:return 0;}}
static int addtab_vid6_batch(unsigned d) {return d==0x57 || d==0xac;}
static int addtab_winbond32_batch(unsigned d) {return d==0x11a;}
static int addtab_mx26_batch(unsigned d) {return d==0xe9;}
static int addtab_sst7_batch(unsigned d) {return d==0xf1 || d==0xf2;}
static int addtab_sanyo5_batch(unsigned d) {return d>=0xeb && d<=0xee;}
static int addtab_exact17_batch(unsigned d,unsigned key) {return d==0xbd||d==0xbe||d==0xc6||d==0xc7||d==0x121||d==0x122||d==0xf0||(d==0x117&&key==125);}
static int addtab_tail9_batch(unsigned d) {return d==0xaf||d==0x120||d==0x13b||d==0x13c;}
static int addtab_tail24_batch(unsigned d) {return d==0x117||d==0x128||d==0x12a||d==0x126||d==0x11b||d==0x11c;}
static int addtab_tail18_batch(unsigned d) {return d==0xc2||d==0xe3||d==0xcf||d==0xd0||d==0xd1;}
static int addtab_gl57_batch(unsigned d) {switch(d){case 0x10f:case 0x110:case 0x111:case 0x112:case 0x113:case 0x118:case 0x119:case 0x11d:case 0x11e:case 0x11f:case 0x123:case 0x124:case 0x125:return 1;default:return 0;}}
static int page_byte21_batch(unsigned cls,unsigned d) {return cls==2041u && d>=0x14 && d<=0x1a;}
#include "../../NewDeviceLib/sources/sst-page/profiles.h"
#include "../../NewDeviceLib/sources/amic4/profiles.h"
#include "../../NewDeviceLib/sources/gdacotp6/profiles.h"
#include "../../NewDeviceLib/sources/ite2/profiles.h"
#include "../../NewDeviceLib/sources/ptc010three/profiles.h"
#include "../../NewDeviceLib/sources/sierra2/profiles.h"
#include "../../NewDeviceLib/sources/tmp1962one/profiles.h"
#include "../../NewDeviceLib/sources/emtc18/profiles.h"
#include "../../NewDeviceLib/sources/nand15/profiles.h"
#include "../../NewDeviceLib/sources/at29-13/profiles.h"
static int addtab_gl45_batch(unsigned d) {switch(d){case 0x10b:case 0x10c:case 0x55:case 0x5c:case 0x5d:case 0x5f:case 0x60:case 0x61:case 0x69:case 0x6d:case 0xaa:case 0xd4:case 0xd6:case 0xd8:case 0xda:case 0xdc:return 1;default:return 0;}}
static int addtab_ew18_batch(unsigned d) {return d==0xb0 || d==0xb1 || d==0xb2;}
static int addtab_micron27_batch(unsigned d) {return d==0xb3 || d==0xb5 || d==0xb6;}
static int addtab_ew54_batch(unsigned d) {return d==0xb4 || d==0xe2 || d==0xe4 || d==0xe5 || d==0xe6;}
static int addtab_gl84_batch(unsigned definition) {
    return definition==0x114 || definition==0x115 || definition==0x127 || definition==0x129;
}
static int addtab_word131_batch(unsigned definition) {
    switch(definition) {
    case 0xe:
    case 0xf:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x42:
    case 0x43:
    case 0x72:
    case 0x73:
    case 0x76:
    case 0x77:
    case 0x81:
    case 0x82:
    case 0x8d:
    case 0x8e:
    case 0x8f:
    case 0x90:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9a:
    case 0xa0:
    case 0xa1:
    case 0xad:
    case 0xae:
    case 0xf5:
    case 0xf6:
    case 0xf7:
    case 0xf8:
    case 0xfb:
    case 0xfc:
    case 0xfd:
    case 0xfe:
    case 0xff:
    case 0x101:
    case 0x105:
    case 0x106:
        return 1;
    default: return 0;
    }
}

typedef struct installation_reference {
    lt48_catalogue_entry entry;
} installation_reference;

struct lt48_installation {
    atomic_size_t references_count;
    char *path;
    lt48_rez *catalogue;
    installation_reference *references;
    size_t device_count;
    size_t catalogue_numeric_column_count;
    lt48_new_device_lib *new_library;
    lt48_new_device_lib_report new_report;
    struct { const lt48_new_device_record *record; size_t donor; } *new_devices;
    size_t new_count;
    const lt48_new_device_record **corrections;
    const char *source_profile; /* Immutable catalogue identity, computed once. */
};

static const char *installation_compute_source_profile(const lt48_installation *installation);

static const char *installation_definition_stem(
    const lt48_installation *installation, uint16_t device_class
) {
    return lt48_definition_stem_for_class_format(
        device_class, installation->catalogue_numeric_column_count
    );
}

lt48_installation *lt48_installation_retain(lt48_installation *installation) {
    if (installation) atomic_fetch_add_explicit(&installation->references_count, 1u, memory_order_relaxed);
    return installation;
}

void lt48_installation_close(lt48_installation *installation) {
    if (installation == NULL) {
        return;
    }
    if (atomic_fetch_sub_explicit(&installation->references_count, 1u, memory_order_acq_rel) != 1u) return;
    lt48_new_device_lib_close(installation->new_library);
    free(installation->new_devices);
    free(installation->corrections);
    free(installation->references);
    free(installation->path);
    lt48_rez_close(installation->catalogue);
    free(installation);
}

static lt48_status installation_load_new_library(lt48_installation *x);

lt48_status lt48_installation_open(
    const char *path, lt48_installation **out_installation
) {
    static const char suffix[] = "/dev48uxp.rez";
    lt48_installation *installation = NULL;
    lt48_rez_info info;
    lt48_status status;
    char *catalogue_path = NULL;
    size_t path_length;
    size_t device_count = 0;
    size_t next = 0;
    char *cache_manifest_path = NULL;
    struct stat cache_marker;

    if (path == NULL || out_installation == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_installation = NULL;
    if (strlen(path) > SIZE_MAX - sizeof("/manifest.lt48")) {
        return LT48_INVALID_ARGUMENT;
    }
    cache_manifest_path = malloc(strlen(path) + sizeof("/manifest.lt48"));
    if (cache_manifest_path == NULL) return LT48_NO_MEMORY;
    snprintf(cache_manifest_path, strlen(path) + sizeof("/manifest.lt48"),
             "%s/manifest.lt48", path);
    if (lstat(cache_manifest_path, &cache_marker) == 0) {
        lt48_private_cache_report cache_report;
        status = lt48_private_cache_verify(path, &cache_report);
        free(cache_manifest_path);
        if (status != LT48_OK) return status;
    } else {
        if (errno != ENOENT) {
            free(cache_manifest_path);
            return LT48_FILE_ERROR;
        }
        snprintf(cache_manifest_path, strlen(path) + sizeof("/manifest.lt48"),
                 "%s/objects", path);
        if (lstat(cache_manifest_path, &cache_marker) == 0) {
            free(cache_manifest_path);
            return LT48_FORMAT_ERROR;
        }
        free(cache_manifest_path);
    }
    if (strlen(path) > SIZE_MAX - sizeof(suffix)) {
        return LT48_INVALID_ARGUMENT;
    }
    path_length = strlen(path) + sizeof(suffix);
    catalogue_path = malloc(path_length);
    installation = calloc(1, sizeof(*installation));
    if (catalogue_path == NULL || installation == NULL) {
        free(catalogue_path);
        free(installation);
        return LT48_NO_MEMORY;
    }
    atomic_init(&installation->references_count, 1u);
    installation->path = malloc(strlen(path) + 1);
    if (installation->path == NULL) {
        free(catalogue_path);
        lt48_installation_close(installation);
        return LT48_NO_MEMORY;
    }
    memcpy(installation->path, path, strlen(path) + 1);
    snprintf(catalogue_path, path_length, "%s%s", path, suffix);
    status = lt48_rez_open(catalogue_path, &installation->catalogue);
    free(catalogue_path);
    if (status != LT48_OK) {
        lt48_installation_close(installation);
        return status;
    }
    status = lt48_rez_get_info(installation->catalogue, &info);
    if (status != LT48_OK) {
        lt48_installation_close(installation);
        return status;
    }
    for (size_t resource_index = 0; resource_index < info.resource_count;
         ++resource_index) {
        lt48_rez_resource resource;
        lt48_catalogue_entry ignored;
        size_t count = 0;
        status = lt48_rez_get_resource(
            installation->catalogue, resource_index, &resource
        );
        if (status != LT48_OK) {
            lt48_installation_close(installation);
            return status;
        }
        if (strcmp(resource.type_name, "TDevPool") != 0) {
            continue;
        }
        status = lt48_rez_get_catalogue_entry(
            installation->catalogue, resource_index, 0, &ignored, &count
        );
        if (status != LT48_OK && !(status == LT48_NOT_FOUND && count == 0)) {
            lt48_installation_close(installation);
            return status;
        }
        if (count != 0u) {
            if (installation->catalogue_numeric_column_count == 0u)
                installation->catalogue_numeric_column_count =
                    ignored.numeric_column_count;
            else if (installation->catalogue_numeric_column_count !=
                     ignored.numeric_column_count) {
                lt48_installation_close(installation);
                return LT48_FORMAT_ERROR;
            }
        }
        if (count > SIZE_MAX - device_count) {
            lt48_installation_close(installation);
            return LT48_FORMAT_ERROR;
        }
        device_count += count;
    }
    if (device_count > SIZE_MAX / sizeof(*installation->references)) {
        lt48_installation_close(installation);
        return LT48_FORMAT_ERROR;
    }
    installation->references = calloc(
        device_count == 0 ? 1 : device_count, sizeof(*installation->references)
    );
    if (installation->references == NULL) {
        lt48_installation_close(installation);
        return LT48_NO_MEMORY;
    }
    installation->device_count = device_count;
    for (size_t resource_index = 0; resource_index < info.resource_count;
         ++resource_index) {
        lt48_rez_resource resource;
        lt48_catalogue_entry ignored;
        size_t count = 0;
        status = lt48_rez_get_resource(
            installation->catalogue, resource_index, &resource
        );
        if (status != LT48_OK) {
            lt48_installation_close(installation);
            return status;
        }
        if (strcmp(resource.type_name, "TDevPool") != 0) {
            continue;
        }
        status = lt48_rez_get_catalogue_entry(
            installation->catalogue, resource_index, 0, &ignored, &count
        );
        if (status != LT48_OK && !(status == LT48_NOT_FOUND && count == 0)) {
            lt48_installation_close(installation);
            return status;
        }
        for (size_t device_index = 0; device_index < count; ++device_index) {
            /* Catalogue bytes remain immutable for the installation lifetime.
             * Decode once: scanning each vendor's name pool on every lookup
             * made audit/search/row rendering quadratic. */
            status = lt48_rez_get_catalogue_entry(installation->catalogue,
                resource_index, device_index, &installation->references[next].entry, NULL);
            if (status != LT48_OK) {
                lt48_installation_close(installation);
                return status;
            }
            ++next;
        }
    }
    installation->source_profile=installation_compute_source_profile(installation);
    status=installation_load_new_library(installation);
    if(status!=LT48_OK){lt48_installation_close(installation);return status;}
    *out_installation = installation;
    return LT48_OK;
}

size_t lt48_installation_device_count(const lt48_installation *installation) {
    return installation ? installation->device_count+installation->new_count : 0;
}

static const char *installation_compute_source_profile(const lt48_installation *installation) {
    unsigned char digest[LT48_SHA256_SIZE];
    char hex[LT48_SHA256_SIZE * 2u + 1u];
    if (!installation || lt48_rez_sha256(installation->catalogue, digest) != LT48_OK)
        return "unrecognized";
    for (size_t i = 0; i < sizeof(digest); ++i)
        snprintf(hex + 2u*i, 3u, "%02x", digest[i]);
    if (!strcmp(hex, "a462dbeb2329a7b993e828447db8b43914c63ad82e9e36e95b2edd66b29cf85c"))
        return "AEC 14.30";
    if (!strcmp(hex, "199263b340c19eee9513594162c7c6b00949fdf4a420f0ea0ffa5630de5162af"))
        return "AEC 14.40";
    if (!strcmp(hex, "a1f539dee05f744739a739c69e61a552c50bfbd94e9e42dc774c8b89db00234f"))
        return "Dataman 13.10";
    if (!strcmp(hex, "b1bc1a94c6f9f455da64b14573b9a2345cea76fea05f51976fc0df6fb7188b4c"))
        return "AEC 7.60";
    return "unrecognized";
}

const char *lt48_installation_source_profile(const lt48_installation *installation) {
    return installation && installation->source_profile ? installation->source_profile : "unrecognized";
}

const char *lt48_installation_path(const lt48_installation *installation) {
    return installation == NULL ? NULL : installation->path;
}

static const char *installation_property_filename(
    lt48_property_sheet_kind kind
) {
    switch (kind) {
    case LT48_PROPERTY_SHEET_CONFIGURATION: return "configuration.json";
    case LT48_PROPERTY_SHEET_FUSE_MAP: return "fuse-map.json";
    case LT48_PROPERTY_SHEET_TEST_VECTOR: return "test-vector.json";
    default: return NULL;
    }
}

lt48_status lt48_installation_get_device(
    const lt48_installation *installation,
    size_t index,
    lt48_catalogue_entry *out_entry
) {
    const installation_reference *reference;
    if (installation == NULL || out_entry == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if(index>=installation->device_count && index-installation->device_count<installation->new_count) {
        size_t n=index-installation->device_count;
        const lt48_new_device_record *r=installation->new_devices[n].record;
        lt48_status st=lt48_installation_get_device(installation,installation->new_devices[n].donor,out_entry);
        if(st==LT48_OK) {
            out_entry->vendor=r->manufacturer;out_entry->device_name=(const unsigned char*)r->part;out_entry->device_name_length=strlen(r->part);
            if(r->override_mask&1)out_entry->device_type=r->device_type;
            if(r->override_mask&32)out_entry->manufacturer_id=r->manufacturer_id;
            if(r->override_mask&64)out_entry->device_id=r->device_id;
        }
        return st;
    }
    if (index >= installation->device_count) {
        return LT48_NOT_FOUND;
    }
    reference = &installation->references[index];
    *out_entry = reference->entry;
    return LT48_OK;
}

static void installation_audit_record_failure(
    lt48_installation_audit *audit, size_t device_index, lt48_status status
) {
    ++audit->failed_device_count;
    if (audit->first_failed_device_index == SIZE_MAX) {
        audit->first_failed_device_index = device_index;
        audit->first_failure = status;
    }
}

lt48_status lt48_installation_audit_all(
    const lt48_installation *installation,
    lt48_installation_audit *out_audit
) {
    return lt48_installation_audit_all_cancellable(
        installation, NULL, NULL, out_audit);
}

static lt48_status installation_audit_cancelled(
    lt48_installation_audit *audit
) {
    memset(audit, 0, sizeof(*audit));
    audit->first_failed_device_index = SIZE_MAX;
    audit->first_failure = LT48_CANCELLED;
    return LT48_CANCELLED;
}

lt48_status lt48_installation_audit_all_cancellable(
    const lt48_installation *installation,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_installation_audit *out_audit
) {
    enum { FIRST_CLASS = 2000, MAX_CLASS_COUNT = 49 };
    int class_used[MAX_CLASS_COUNT] = {0};
    size_t class_count;
    unsigned char manifest[LT48_SHA256_SIZE + MAX_CLASS_COUNT *
                           (2u + LT48_SHA256_SIZE)];
    size_t manifest_size = 0u;
    lt48_status status;
    if (installation == NULL || out_audit == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_audit, 0, sizeof(*out_audit));
    class_count = installation->catalogue_numeric_column_count >= 15u ? 49u : 48u;
    out_audit->catalogue_device_count = installation->device_count;
    out_audit->first_failed_device_index = SIZE_MAX;
    out_audit->first_failure = LT48_OK;
    if (cancelled != NULL && cancelled(cancel_context))
        return installation_audit_cancelled(out_audit);
    status = lt48_rez_sha256(
        installation->catalogue, manifest + manifest_size
    );
    if (status != LT48_OK) return status;
    manifest_size += LT48_SHA256_SIZE;

    for (size_t index = 0u; index < installation->device_count; ++index) {
        lt48_catalogue_entry entry;
        if (cancelled != NULL && cancelled(cancel_context))
            return installation_audit_cancelled(out_audit);
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) {
            installation_audit_record_failure(out_audit, index, status);
            continue;
        }
        if (entry.device_class < FIRST_CLASS ||
            entry.device_class >= FIRST_CLASS + class_count ||
            installation_definition_stem(installation, entry.device_class) == NULL) {
            ++out_audit->unsupported_class_count;
            installation_audit_record_failure(
                out_audit, index, LT48_NOT_FOUND
            );
            continue;
        }
        class_used[entry.device_class - FIRST_CLASS] = 1;
        ++out_audit->supported_device_count;
    }

    for (size_t class_offset = 0u; class_offset < class_count;
         ++class_offset) {
        uint16_t device_class = (uint16_t)(FIRST_CLASS + class_offset);
        const char *stem;
        char *path;
        size_t path_size;
        lt48_rez *rez = NULL;
        unsigned char digest[LT48_SHA256_SIZE];
        if (cancelled != NULL && cancelled(cancel_context))
            return installation_audit_cancelled(out_audit);
        if (!class_used[class_offset]) continue;
        stem = installation_definition_stem(installation, device_class);
        path_size = strlen(installation->path) + strlen(stem) + 6u;
        path = malloc(path_size);
        if (path == NULL) return LT48_NO_MEMORY;
        snprintf(path, path_size, "%s/%s.rez", installation->path, stem);
        status = lt48_rez_open(path, &rez);
        free(path);
        if (status != LT48_OK) {
            for (size_t index = 0u; index < installation->device_count; ++index) {
                lt48_catalogue_entry entry;
                if (cancelled != NULL && cancelled(cancel_context))
                    return installation_audit_cancelled(out_audit);
                if (lt48_installation_get_device(
                        installation, index, &entry) == LT48_OK &&
                    entry.device_class == device_class)
                    installation_audit_record_failure(
                        out_audit, index, status
                    );
            }
            continue;
        }
        ++out_audit->definition_file_count;
        status = lt48_rez_sha256(rez, digest);
        if (status != LT48_OK) {
            lt48_rez_close(rez);
            return status;
        }
        manifest[manifest_size++] = (unsigned char)(device_class >> 8);
        manifest[manifest_size++] = (unsigned char)device_class;
        memcpy(manifest + manifest_size, digest, LT48_SHA256_SIZE);
        manifest_size += LT48_SHA256_SIZE;

        for (size_t index = 0u; index < installation->device_count; ++index) {
            lt48_catalogue_entry entry;
            lt48_device_definition definition;
            lt48_family_info family;
            lt48_pin_code pin;
            char name[16];
            size_t resource_index;
            if (cancelled != NULL && cancelled(cancel_context)) {
                lt48_rez_close(rez);
                return installation_audit_cancelled(out_audit);
            }
            status = lt48_installation_get_device(
                installation, index, &entry
            );
            if (status != LT48_OK || entry.device_class != device_class)
                continue;
            snprintf(name, sizeof(name), "D%x", entry.device_type);
            status = lt48_rez_find_resource(rez, name, &resource_index);
            if (status == LT48_OK)
                status = lt48_rez_get_device_definition(
                    rez, resource_index, &definition
                );
            if (status == LT48_OK) {
                snprintf(name, sizeof(name), "M%x", definition.family_code);
                status = lt48_rez_find_resource(rez, name, &resource_index);
            }
            if (status == LT48_OK)
                status = lt48_rez_get_family_info(
                    rez, resource_index, &family
                );
            if (status == LT48_OK) {
                snprintf(name, sizeof(name), "P%x", definition.pinmap);
                status = lt48_rez_find_resource(rez, name, &resource_index);
            }
            if (status == LT48_OK)
                status = lt48_rez_get_pin_code(rez, resource_index, &pin);
            if (status == LT48_OK) {
                ++out_audit->resolved_device_count;
                if (pin.pin_count == 0u) ++out_audit->zero_pin_device_count;
            } else {
                installation_audit_record_failure(out_audit, index, status);
            }
        }
        lt48_rez_close(rez);
    }
    if (cancelled != NULL && cancelled(cancel_context))
        return installation_audit_cancelled(out_audit);
    lt48_sha256(manifest, manifest_size, out_audit->manifest_sha256);
    return out_audit->failed_device_count == 0u ?
           LT48_OK : out_audit->first_failure;
}

lt48_status lt48_installation_resolve_device(
    const lt48_installation *installation,
    size_t index,
    lt48_device_spec *out_spec
) {
    lt48_catalogue_entry entry;
    lt48_device_definition definition;
    lt48_family_info family;
    lt48_pin_code pin;
    lt48_rez *rez = NULL;
    lt48_status status;
    const char *stem;
    char resource_name[16];
    char related_name[16];
    char *path;
    size_t path_length;
    size_t resource_index;
    uint32_t capacity = 0;

    if (installation == NULL || out_spec == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if(index>=installation->device_count && index-installation->device_count<installation->new_count) {
        size_t n=index-installation->device_count;
        const lt48_new_device_record *r=installation->new_devices[n].record;
        status=lt48_installation_resolve_device(installation,installation->new_devices[n].donor,out_spec);
        if(status==LT48_OK) {
            status=lt48_installation_get_device(installation,index,&out_spec->catalogue);
            if(r->override_mask&2)out_spec->definition_resource=r->definition;
            if(r->override_mask&4)out_spec->pinmap=r->pinmap;
            if(r->override_mask&8)out_spec->pin_count=r->pins;
            if(r->override_mask&16) {
                if(!out_spec->element_bits||((uint64_t)r->capacity*8)%out_spec->element_bits)return LT48_FORMAT_ERROR;
                out_spec->capacity_bytes=r->capacity;out_spec->element_count=(uint64_t)r->capacity*8/out_spec->element_bits;
            }
        }
        return status;
    }
    status = lt48_installation_get_device(installation, index, &entry);
    if (status != LT48_OK) {
        return status;
    }
    stem = installation_definition_stem(installation, entry.device_class);
    if (stem == NULL) {
        return LT48_NOT_FOUND;
    }
    if (strlen(installation->path) > SIZE_MAX - strlen(stem) - 7) {
        return LT48_INVALID_ARGUMENT;
    }
    path_length = strlen(installation->path) + 1 + strlen(stem) + 4 + 1;
    path = malloc(path_length);
    if (path == NULL) {
        return LT48_NO_MEMORY;
    }
    snprintf(path, path_length, "%s/%s.rez", installation->path, stem);
    status = lt48_rez_open(path, &rez);
    free(path);
    if (status != LT48_OK) {
        return status;
    }
    snprintf(resource_name, sizeof(resource_name), "D%x", entry.device_type);
    status = lt48_rez_find_resource(rez, resource_name, &resource_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_device_definition(rez, resource_index, &definition);
    }
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return status;
    }
    snprintf(related_name, sizeof(related_name), "M%x", definition.family_code);
    status = lt48_rez_find_resource(rez, related_name, &resource_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_family_info(rez, resource_index, &family);
    }
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return status;
    }
    snprintf(related_name, sizeof(related_name), "P%x", definition.pinmap);
    status = lt48_rez_find_resource(rez, related_name, &resource_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_pin_code(rez, resource_index, &pin);
    }
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return status;
    }
    if (definition.has_device_size && !definition.has_element_geometry) {
        capacity = definition.device_size;
    } else if (definition.has_element_geometry) {
        uint32_t bytes_per_element;
        if (definition.element_count == 0U ||
            definition.element_bits == 0U) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        bytes_per_element = ((uint32_t)definition.element_bits + 7U) / 8U;
        if (definition.element_count > UINT32_MAX / bytes_per_element) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        capacity = definition.element_count * bytes_per_element;
    }
    /* Dataman 13.10 D130 declares 352 words although M99 transmits only
     * eight address bits. ST93CS66/67 are 256 x 16. Bind this correction to
     * the exact source digest; leave other releases' definitions untouched. */
    if(entry.device_class==2000 && entry.device_type==0x130 &&
       definition.family_code==0x99 && definition.element_bits==16 &&
       definition.element_count==352 && !strcmp(stem,"GA_____")) {
        unsigned char digest[32];char hex[65];
        if(lt48_rez_sha256(rez,digest)==LT48_OK) {
            for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
            if(!strcmp(hex,"a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad")) {
                definition.element_count=256;capacity=512;
            }
        }
    }
    /* Exact class-2026 TPOFCoreCode data spans. POF headers are not device data. */
    if(entry.device_class==2026u && !strcmp(stem,"GC_____")) {
        unsigned char digest[32]; char hex[65];
        if(lt48_rez_sha256(rez,digest)==LT48_OK){
            for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
            if(!strcmp(hex,LT48_FAMILY_EPC_OTP_SOURCE_SHA256)) {
                const lt48_family_tuple *tables[]={lt48_family_epc_otp_tuples,lt48_family_epc2_tuples};
                const size_t counts[]={sizeof(lt48_family_epc_otp_tuples)/sizeof(lt48_family_tuple),sizeof(lt48_family_epc2_tuples)/sizeof(lt48_family_tuple)};
                for(unsigned t=0;t<2;t++)for(size_t k=0;k<counts[t];k++){
                    const lt48_family_tuple *v=&tables[t][k];
                    if(v->definition==entry.device_type&&v->family==definition.family_code&&v->pinmap==definition.pinmap){capacity=v->capacity;definition.element_count=capacity;definition.element_bits=8;}
                }
            }
        }
    }
    /* POF logic images use the selected core's packed byte span, not the
     * catalogue's generic fuse geometry or the POF file/container size. */
    if (definition.kind == LT48_DEVICE_DEFINITION_POF &&
        ((entry.device_class == 2026u && !strcmp(stem, "GC_____")) ||
         (entry.device_class == 2028u && !strcmp(stem, "GCB____")))) {
        const unsigned tuples[][4] = {
            {2026,0,0,0}, {2026,2,2,2}, {2026,3,3,3}, {2026,4,4,4},
            {2026,5,5,5}, {2026,16,16,8}, {2026,17,17,9},
            {2028,0,0,0}, {2028,1,1,1}
        };
        int selected = 0;
        for (size_t k = 0; k < sizeof tuples / sizeof tuples[0]; k++)
            if (entry.device_class == tuples[k][0] && entry.device_type == tuples[k][1] &&
                definition.family_code == tuples[k][2] && definition.pinmap == tuples[k][3])
                selected = 1;
        unsigned char digest[32]; char hex[65];
        if (selected && lt48_rez_sha256(rez, digest) == LT48_OK) {
            for (unsigned k = 0; k < 32; k++) snprintf(hex + 2*k, 3, "%02x", digest[k]);
            const char *expected = entry.device_class == 2026u ?
                "fc6529b7b6c6443dc9a962052b5f18e7f2928ae1bdfc3248d373a9a9fec32c20" :
                "9bd0f6c9cd3e5e6888a23f8750c6acc6da7fafb64501f477e064db3c8bb74b12";
            if (!strcmp(hex, expected)) {
                lt48_pof_core core;
                status = lt48_rez_find_resource(rez, resource_name, &resource_index);
                if (status == LT48_OK) status = lt48_rez_get_pof_core(rez, resource_index, &core);
                if (status != LT48_OK || !core.total_bytes || core.total_bytes > UINT32_MAX) {
                    lt48_rez_close(rez);
                    return status == LT48_OK ? LT48_FORMAT_ERROR : status;
                }
                capacity = (uint32_t)core.total_bytes;
                definition.element_count = capacity;
                definition.element_bits = 8;
            }
        }
    }
    if(definition.kind==LT48_DEVICE_DEFINITION_XILINX_MEMORY && definition.element_bits && definition.element_bits<=16)
        capacity=definition.element_count+((definition.element_bits+7)/8-1)*65536u;
    memset(out_spec, 0, sizeof(*out_spec));
    out_spec->catalogue = entry;
    out_spec->definition_stem = stem;
    out_spec->definition_kind = definition.kind;
    out_spec->definition_resource = entry.device_type;
    out_spec->family_code = definition.family_code;
    out_spec->pinmap = definition.pinmap;
    out_spec->pin_count = pin.pin_count;
    if(entry.device_class==2048u && (entry.device_type==0x20b || entry.device_type==0x20c) &&
       definition.pinmap==0xc4 && (definition.family_code==0x176 || definition.family_code==0x177))capacity=0xf00100u;
    if(entry.device_class==2048u) {
        const lt48_pic18_admission *admitted=lt48_pic18_admission_find(entry.device_type,definition.family_code,
            definition.pinmap,entry.device_name,entry.device_name_length);
        if(admitted)capacity=admitted->capacity;
    }
    unsigned r8cap=lt48_r8c8_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(r8cap)capacity=r8cap;
    unsigned mspcapacity=lt48_msp43076_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(mspcapacity)capacity=mspcapacity;
    unsigned s8capacity=lt48_stm8s4_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(s8capacity)capacity=s8capacity;
    unsigned c25capacity=lt48_cc254x5_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(c25capacity)capacity=c25capacity;
    unsigned sy80capacity=lt48_sy80_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sy80capacity)capacity=sy80capacity;
    unsigned sy76capacity=lt48_sy76_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sy76capacity)capacity=sy76capacity;
    unsigned mw36capacity=lt48_mw36_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(mw36capacity)capacity=mw36capacity;
    unsigned sm30capacity=lt48_sm30_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sm30capacity)capacity=sm30capacity;
    unsigned sx30capacity=lt48_sx30_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sx30capacity)capacity=sx30capacity;
    unsigned mg15capacity=lt48_mg15_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(mg15capacity)capacity=mg15capacity;
    unsigned sn9capacity=lt48_sn9_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sn9capacity)capacity=sn9capacity;
    unsigned ob16capacity=lt48_ob16_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(ob16capacity)capacity=ob16capacity;
    unsigned wt10capacity=lt48_wt10_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(wt10capacity)capacity=wt10capacity;
    unsigned wt8capacity=lt48_wt8_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(wt8capacity)capacity=wt8capacity;
    unsigned hy14capacity=lt48_hy14_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(hy14capacity)capacity=hy14capacity;
    unsigned mw6capacity=lt48_mw6_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(mw6capacity)capacity=mw6capacity;
    unsigned my7capacity=lt48_my7_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(my7capacity)capacity=my7capacity;
    unsigned my4capacity=lt48_my4_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(my4capacity)capacity=my4capacity;
    unsigned sc2capacity=lt48_sc2_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sc2capacity)capacity=sc2capacity;
    unsigned sc4gcapacity=lt48_sc4g_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sc4gcapacity)capacity=sc4gcapacity;
    unsigned sc4acapacity=lt48_sc4a_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sc4acapacity)capacity=sc4acapacity;
    unsigned tp4capacity=lt48_tp4_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(tp4capacity)capacity=tp4capacity;
    unsigned tp6capacity=lt48_tp6_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(tp6capacity)capacity=tp6capacity;
    unsigned mw8capacity=lt48_mw8_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(mw8capacity)capacity=mw8capacity;
    unsigned gc8capacity=lt48_gc8_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(gc8capacity)capacity=gc8capacity;
    unsigned ab11capacity=lt48_ab11_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(ab11capacity)capacity=ab11capacity;
    unsigned su16capacity=lt48_su16_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(su16capacity)capacity=su16capacity;
    unsigned so30capacity=lt48_so30_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(so30capacity)capacity=so30capacity;
    unsigned sm3capacity=lt48_sm3_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sm3capacity)capacity=sm3capacity;
    unsigned sm6capacity=lt48_sm6_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sm6capacity)capacity=sm6capacity;
    unsigned sc58capacity=lt48_sst58rc2_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(sc58capacity)capacity=sc58capacity;
    unsigned nt25capacity=lt48_novatek25_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(nt25capacity)capacity=nt25capacity;
    unsigned nv8capacity=lt48_nuvoton8_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(nv8capacity)capacity=nv8capacity;
    unsigned x7capacity=lt48_xmega7_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(x7capacity)capacity=x7capacity;
    unsigned smcapacity=lt48_sstmx16_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(smcapacity)capacity=smcapacity;
    unsigned nwcapacity=lt48_n79w79_21_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(nwcapacity)capacity=nwcapacity;
    unsigned nucapacity=lt48_nuvoton30_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(nucapacity)capacity=nucapacity;
    unsigned numicrocapacity=lt48_numicro81_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(numicrocapacity)capacity=numicrocapacity;
    unsigned n79capacity=lt48_n79w79_capacity_internal(entry.device_class,entry.device_type,definition.family_code,definition.pinmap);if(n79capacity)capacity=n79capacity;
    if(entry.device_class==2009&&entry.device_type==0x14&&definition.family_code==11&&definition.pinmap==4){unsigned char dh[32];char sh[65];if(lt48_rez_sha256(rez,dh)==LT48_OK){for(unsigned k=0;k<32;k++)snprintf(sh+2*k,3,"%02x",dh[k]);unsigned corrected=st2009two_geometry(entry.device_class,entry.device_type,definition.family_code,definition.pinmap,capacity,sh);if(corrected!=capacity){capacity=corrected;definition.element_count=corrected;}}}
    out_spec->capacity_bytes = capacity;
    out_spec->element_count = definition.element_count;
    out_spec->element_bits = !strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")?fm8one_element_bits(entry.device_class,entry.device_type,definition.family_code,definition.pinmap,definition.element_bits,capacity):definition.element_bits;
    out_spec->secondary_element_count = definition.secondary_element_count;
    out_spec->secondary_element_bits = definition.secondary_element_bits;
    out_spec->operation_count = family.operation_count;
    out_spec->variable_count = family.variable_count;
    out_spec->configuration_set = family.configuration_set;
    out_spec->family_header_value = family.header_value;
    uint32_t pic_image=lt48_pic_image_capacity_internal(installation,out_spec);
    if(pic_image)out_spec->capacity_bytes=pic_image;
    pic_image=lt48_pic24_image_capacity_internal(installation,out_spec);
    if(pic_image)out_spec->capacity_bytes=pic_image;
    lt48_rez_close(rez);
    return LT48_OK;
}

lt48_status lt48_installation_open_device_property_document(
    const lt48_installation *installation,
    size_t device_index,
    lt48_property_sheet_kind kind,
    lt48_property_document **out_document
) {
    lt48_device_spec spec;
    const char *filename;
    lt48_status status;
    char *path;
    size_t path_size;
    struct stat metadata;
    int count;
    if (installation == NULL || out_document == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_document = NULL;
    filename = installation_property_filename(kind);
    if (filename == NULL && kind != LT48_PROPERTY_SHEET_OPERATION_OPTION)
        return LT48_INVALID_ARGUMENT;
    status = lt48_installation_resolve_device(
        installation, device_index, &spec);
    if (status != LT48_OK) return status;
    if (kind == LT48_PROPERTY_SHEET_OPERATION_OPTION) {
        lt48_rez *rez = NULL;
        lt48_family_operation_options options;
        char family_name[16];
        size_t family_index;
        count = snprintf(
            NULL, 0, "%s/%s.rez", installation->path,
            spec.definition_stem);
        if (count < 0 || (size_t)count == SIZE_MAX)
            return LT48_INVALID_ARGUMENT;
        path_size = (size_t)count + 1u;
        path = malloc(path_size);
        if (path == NULL) return LT48_NO_MEMORY;
        if (snprintf(
                path, path_size, "%s/%s.rez", installation->path,
                spec.definition_stem) != count) {
            free(path);
            return LT48_FORMAT_ERROR;
        }
        status = lt48_rez_open(path, &rez);
        free(path);
        if (status != LT48_OK) return status;
        snprintf(family_name, sizeof(family_name), "M%x", spec.family_code);
        status = lt48_rez_find_resource(rez, family_name, &family_index);
        if (status == LT48_OK)
            status = lt48_rez_get_family_operation_options(
                rez, family_index, &options);
        if (status == LT48_OK)
            status = lt48_property_document_from_operation_options(
                spec.catalogue.device_class, spec.capacity_bytes,
                spec.element_count, &options, out_document);
        lt48_rez_close(rez);
        return status;
    }
    count = snprintf(
        NULL, 0, "%s/ui-properties/%s/D%x/%s",
        installation->path, spec.definition_stem,
        spec.catalogue.device_type, filename);
    if (count < 0 || (size_t)count == SIZE_MAX)
        return LT48_INVALID_ARGUMENT;
    path_size = (size_t)count + 1u;
    path = malloc(path_size);
    if (path == NULL) return LT48_NO_MEMORY;
    if (snprintf(
            path, path_size, "%s/ui-properties/%s/D%x/%s",
            installation->path, spec.definition_stem,
            spec.catalogue.device_type, filename) != count) {
        free(path);
        return LT48_FORMAT_ERROR;
    }
    if (lstat(path, &metadata) != 0) {
        int saved_errno = errno;
        free(path);
        if (saved_errno != ENOENT ||
            kind != LT48_PROPERTY_SHEET_CONFIGURATION ||
            spec.configuration_set == 0u)
            return saved_errno == ENOENT ? LT48_NOT_FOUND : LT48_FILE_ERROR;
        count = snprintf(
            NULL, 0, "%s/ui-properties/configuration-sets/%u/configuration.json",
            installation->path, spec.configuration_set);
        if (count < 0 || (size_t)count == SIZE_MAX)
            return LT48_INVALID_ARGUMENT;
        path_size = (size_t)count + 1u;
        path = malloc(path_size);
        if (path == NULL) return LT48_NO_MEMORY;
        if (snprintf(
                path, path_size,
                "%s/ui-properties/configuration-sets/%u/configuration.json",
                installation->path, spec.configuration_set) != count) {
            free(path);
            return LT48_FORMAT_ERROR;
        }
        if (lstat(path, &metadata) != 0) {
            status = errno == ENOENT ? LT48_NOT_FOUND : LT48_FILE_ERROR;
            free(path);
            return status;
        }
    }
    if (!S_ISREG(metadata.st_mode)) {
        free(path);
        return LT48_FORMAT_ERROR;
    }
    status = lt48_property_document_open(path, out_document);
    free(path);
    return status;
}

static lt48_status installation_find_function_name(
    const lt48_rez *rez, size_t names_index, uint16_t identifier,
    lt48_defined_name *out_name
) {
    size_t count = 0;
    lt48_defined_name match = {0};
    int found = 0;
    lt48_status status = lt48_rez_get_defined_names(rez, names_index, NULL, 0, &count);
    if (status != LT48_BUFFER_TOO_SMALL && status != LT48_OK) return status;
    if (!count) return LT48_NOT_FOUND;
    if (count > SIZE_MAX / sizeof(lt48_defined_name)) return LT48_FORMAT_ERROR;
    lt48_defined_name *names = malloc(count * sizeof(*names));
    if (!names) return LT48_NO_MEMORY;
    status = lt48_rez_get_defined_names(rez, names_index, names, count, &count);
    if (status == LT48_OK) {
        for (size_t index = 0; index < count; ++index) {
            const lt48_defined_name *candidate = &names[index];
            if (candidate->identifier != identifier || candidate->name_length < 5u ||
                memcmp(candidate->name, "_FUN_", 5u) != 0) continue;
            if (found) { status = LT48_FORMAT_ERROR; break; }
            match = *candidate;
            found = 1;
        }
    }
    free(names);
    if (status != LT48_OK) return status;
    if (!found) return LT48_NOT_FOUND;
    *out_name = match;
    return LT48_OK;
}

lt48_status lt48_installation_device_operations(
    const lt48_installation *installation, size_t device_index,
    lt48_device_operation_binding *items, size_t capacity,
    size_t *out_count
) {
    lt48_device_spec spec;
    lt48_rez *rez = NULL;
    lt48_family_info family;
    char *path = NULL;
    char name[16];
    size_t family_index;
    size_t names_index = 0u;
    int have_names;
    lt48_status status;
    if (installation == NULL || out_count == NULL ||
        (items == NULL && capacity != 0u)) return LT48_INVALID_ARGUMENT;
    *out_count = 0u;
    status = lt48_installation_resolve_device(
        installation, device_index, &spec
    );
    if (status != LT48_OK) return status;
    if (strlen(installation->path) > SIZE_MAX - strlen(spec.definition_stem) -
        6u) return LT48_INVALID_ARGUMENT;
    path = malloc(strlen(installation->path) +
                  strlen(spec.definition_stem) + 6u);
    if (path == NULL) return LT48_NO_MEMORY;
    sprintf(path, "%s/%s.rez", installation->path, spec.definition_stem);
    status = lt48_rez_open(path, &rez);
    free(path);
    if (status != LT48_OK) return status;
    snprintf(name, sizeof(name), "M%x", spec.family_code);
    status = lt48_rez_find_resource(rez, name, &family_index);
    if (status == LT48_OK)
        status = lt48_rez_get_family_info(rez, family_index, &family);
    if (status != LT48_OK) goto done;
    *out_count = family.operation_count;
    if (items == NULL || capacity < family.operation_count) {
        status = LT48_BUFFER_TOO_SMALL;
        goto done;
    }
    have_names = lt48_rez_find_resource(
        rez, "DefineName", &names_index
    ) == LT48_OK;
    for (size_t index = 0u; index < family.operation_count; ++index) {
        lt48_family_operation operation;
        lt48_wave_code wave;
        lt48_defined_name symbol;
        size_t wave_index;
        lt48_device_operation_binding *binding = &items[index];
        memset(binding, 0, sizeof(*binding));
        status = lt48_rez_get_family_operation(
            rez, family_index, index, &operation
        );
        if (status != LT48_OK) goto done;
        binding->operation_id = operation.operation_id;
        binding->wave = operation.wave;
        if (have_names) {
            status = installation_find_function_name(
                rez, names_index, operation.operation_id, &symbol
            );
            if (status == LT48_OK) {
                if (symbol.name_length >= sizeof(binding->symbol)) {
                    status = LT48_FORMAT_ERROR;
                    goto done;
                }
                memcpy(binding->symbol, symbol.name, symbol.name_length);
                binding->symbol[symbol.name_length] = '\0';
                binding->has_symbol = 1;
            } else if (status != LT48_NOT_FOUND) {
                goto done;
            }
            status = LT48_OK;
        }
        snprintf(name, sizeof(name), "W%x", operation.wave);
        if (lt48_rez_find_resource(rez, name, &wave_index) == LT48_OK &&
            lt48_rez_get_wave_code(rez, wave_index, &wave) == LT48_OK) {
            binding->has_wave = 1;
            binding->wave_type = wave.wave_type;
            binding->stored_data_size = wave.data_size;
            binding->has_applet_header = wave.has_applet_header;
            binding->load_address = wave.load_address;
        }
    }
    status = LT48_OK;
done:
    lt48_rez_close(rez);
    return status;
}

static int package_code_in(
    const char *code, const char *const *items, size_t count
) {
    for (size_t index = 0u; index < count; ++index)
        if (strcmp(code, items[index]) == 0) return 1;
    return 0;
}

const char *lt48_installation_device_corrected_name(
    const lt48_installation *installation, size_t device_index
) {
    lt48_catalogue_entry entry;size_t match=SIZE_MAX;
    if(!installation||lt48_installation_get_device(installation,device_index,&entry)!=LT48_OK||
       entry.device_class!=2015||!entry.vendor||strcmp(entry.vendor,"Microchip"))return NULL;
    for(size_t i=0;i<sizeof(lt48_pic_package_labels)/sizeof(*lt48_pic_package_labels);i++){
        size_t n=strlen(lt48_pic_package_labels[i].original);
        if(entry.device_name&&entry.device_name_length==n&&!memcmp(entry.device_name,lt48_pic_package_labels[i].original,n)){match=i;break;}
    }
    if(match==SIZE_MAX)return NULL;
    lt48_device_spec spec;char path[4096],hex[65];unsigned char hash[32];lt48_rez *rez=NULL;
    if(lt48_installation_resolve_device(installation,device_index,&spec)!=LT48_OK||spec.pin_count!=14||
       !spec.definition_stem||strcmp(spec.definition_stem,"GADC___")||
       spec.definition_resource!=lt48_pic_package_labels[match].definition||
       spec.family_code!=lt48_pic_package_labels[match].family||spec.pinmap!=lt48_pic_package_labels[match].pinmap)return NULL;
    if(snprintf(path,sizeof(path),"%s/%s.rez",installation->path,spec.definition_stem)>=(int)sizeof(path)||
       lt48_rez_open(path,&rez)!=LT48_OK)return NULL;
    lt48_status st=lt48_rez_sha256(rez,hash);lt48_rez_close(rez);if(st!=LT48_OK)return NULL;
    for(size_t i=0;i<sizeof(hash);i++)snprintf(hex+2*i,3,"%02x",hash[i]);
    return strcmp(hex,LT48_PIC_PACKAGE_LABEL_SHA256)?NULL:lt48_pic_package_labels[match].corrected;
}

lt48_status lt48_installation_device_guidance(
    const lt48_installation *installation, size_t device_index,
    lt48_device_guidance *out_guidance
) {
    static const char *const direct[] = {"DIP", "SDIP"};
    static const char *const smd[] = {
        "BGA", "FBGA", "LQ", "MLF", "PLCC", "Q", "QFN", "QFP",
        "SO", "SOIC", "SON", "SOT", "SS", "SSOP", "TQ", "TS",
        "TFBGA", "TSS", "TSSOP", "TSW", "VQ", "WFBGA", "WSON"
    };
    lt48_device_spec spec;
    const unsigned char *name;
    size_t length;
    size_t marker_start = SIZE_MAX;
    lt48_status status;
    if (installation == NULL || out_guidance == NULL)
        return LT48_INVALID_ARGUMENT;
    status = lt48_installation_resolve_device(
        installation, device_index, &spec
    );
    if (status != LT48_OK) return status;
    memset(out_guidance, 0, sizeof(*out_guidance));
    out_guidance->definition_pin_count = spec.pin_count;
    out_guidance->adapter_requirement = LT48_ADAPTER_UNKNOWN;
    name = spec.catalogue.device_name;
    length = spec.catalogue.device_name_length;
    const char *corrected=lt48_installation_device_corrected_name(installation,device_index);
    if(corrected){name=(const unsigned char *)corrected;length=strlen(corrected);}
    if (name != NULL && length >= 4u) {
        for (size_t index = length - 1u; index > 0u; --index) {
            if (name[index - 1u] == ' ' && name[index] == '*') {
                marker_start = index;
                break;
            }
        }
    }
    if (marker_start != SIZE_MAX) {
        size_t cursor = marker_start + 1u;
        unsigned pins = 0u;
        size_t digit_count = 0u;
        size_t code_length;
        while (cursor < length && digit_count < 3u &&
               isdigit((unsigned char)name[cursor])) {
            pins = pins * 10u + (unsigned)(name[cursor] - '0');
            ++cursor;
            ++digit_count;
        }
        code_length = length - cursor;
        if (digit_count != 0u && pins <= UINT16_MAX && code_length != 0u &&
            code_length < sizeof(out_guidance->catalogue_package_code) &&
            isalpha((unsigned char)name[cursor])) {
            int valid = 1;
            for (size_t index = cursor; index < length; ++index) {
                unsigned char value = name[index];
                if (!isalnum(value) && value != '-') valid = 0;
            }
            if (valid) {
                out_guidance->has_catalogue_package_hint = 1;
                out_guidance->catalogue_package_pin_count = (uint16_t)pins;
                for (size_t index = 0u; index < code_length; ++index)
                    out_guidance->catalogue_package_code[index] =
                        (char)toupper((unsigned char)name[cursor + index]);
                out_guidance->catalogue_package_code[code_length] = '\0';
            }
        }
    }
    if (out_guidance->has_catalogue_package_hint) {
        if (package_code_in(out_guidance->catalogue_package_code, direct,
                            sizeof(direct) / sizeof(direct[0])))
            out_guidance->adapter_requirement =
                LT48_ADAPTER_POSSIBLY_DIRECT;
        else if (package_code_in(out_guidance->catalogue_package_code, smd,
                                 sizeof(smd) / sizeof(smd[0])))
            out_guidance->adapter_requirement = LT48_ADAPTER_REQUIRED;
        out_guidance->package_pin_count_conflict =
            out_guidance->catalogue_package_pin_count != spec.pin_count;
    }
    if (spec.pin_count != 0u && spec.pin_count <= 48u &&
        !out_guidance->package_pin_count_conflict) {
        unsigned center = (48u - spec.pin_count) / 2u;
        out_guidance->has_socket_span = 1;
        out_guidance->first_socket_position = (uint8_t)(center + 1u);
        out_guidance->last_socket_position =
            (uint8_t)(center + spec.pin_count);
    }
    out_guidance->orientation_known = 0;
    return LT48_OK;
}

static const lt48_known_carrier *installation_known_carrier(const lt48_installation *x,size_t device) {
 lt48_device_spec spec;if(!x||lt48_installation_resolve_device(x,device,&spec)!=LT48_OK||spec.catalogue.device_class!=2015u||!spec.definition_stem||strcmp(spec.definition_stem,"GADC___")||!spec.catalogue.vendor||strcmp(spec.catalogue.vendor,"Microchip")||!spec.catalogue.device_name||spec.catalogue.device_name_length>200)return NULL;
 char key[256],path[4096],hex[65];int n=snprintf(key,sizeof(key),"Microchip:%.*s",(int)spec.catalogue.device_name_length,spec.catalogue.device_name);if(n<0||(size_t)n>=sizeof(key))return NULL;
 const lt48_known_carrier *match=NULL;
 for(size_t i=0;i<sizeof(known_carriers)/sizeof(*known_carriers);i++){const lt48_known_carrier *r=known_carriers+i;if(r->definition==spec.definition_resource&&r->family==spec.family_code&&r->pinmap==spec.pinmap&&!strcmp(r->key,key))match=r;}
 if(!match||spec.pin_count!=match->carrier_pins)return NULL;
 n=snprintf(path,sizeof(path),"%s/GADC___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof(path))return NULL;
 lt48_rez *rez=NULL;unsigned char hash[32];if(lt48_rez_open(path,&rez)!=LT48_OK)return NULL;lt48_status st=lt48_rez_sha256(rez,hash);lt48_rez_close(rez);if(st!=LT48_OK)return NULL;
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",hash[i]);
 return lt48_known_carrier_find(key,spec.definition_resource,spec.family_code,spec.pinmap,hex);
}
lt48_status lt48_installation_device_carrier_info(const lt48_installation *x,size_t device,lt48_device_carrier_info *out) {
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));const lt48_known_carrier *r=installation_known_carrier(x,device);if(!r)return LT48_UNSUPPORTED;
 *out=(lt48_device_carrier_info){r->package,r->note,r->physical_pins,r->carrier_pins,r->dip1_zif,r->net_count,1};return LT48_OK;
}
lt48_status lt48_installation_device_carrier_connection(const lt48_installation *x,size_t device,size_t index,lt48_device_carrier_connection *out) {
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));const lt48_known_carrier *r=installation_known_carrier(x,device);if(!r)return LT48_UNSUPPORTED;
 if(index>=r->net_count)return LT48_INVALID_ARGUMENT;
 *out=(lt48_device_carrier_connection){r->nets[index].chip,r->nets[index].dip,r->nets[index].role};return LT48_OK;
}
const char *lt48_installation_device_adapter_wiring_note(
    const lt48_installation *installation, size_t device_index
) {
    lt48_device_spec spec;
    lt48_rez *rez = NULL;
    unsigned char hash[32];
    char path[4096], hex[65];
    size_t matched = SIZE_MAX;
    if (installation == NULL || lt48_installation_resolve_device(
            installation, device_index, &spec) != LT48_OK ||
        spec.definition_stem == NULL) return NULL;
    if(spec.catalogue.device_class==2048u) {
        lt48_pic18_resources *pic=NULL;
        if(lt48_pic18_open(installation,device_index,&pic)==LT48_OK&&lt48_pic18_is_j_series(pic)) {
            lt48_pic18_j_owner *j=NULL;const char *note=NULL;
            if(lt48_pic18_j_owner_open(installation,device_index,&j)==LT48_OK)note=lt48_pic18_j_owner_adapter(j)->note;
            lt48_pic18_j_owner_close(j);lt48_pic18_close(pic);return note;
        }
        lt48_pic18_close(pic);
    }
    const lt48_known_carrier *authored=installation_known_carrier(installation,device_index);
    if(authored)return authored->note;
    const char *baseline_note=lt48_pic_baseline12_adapter_note_for_spec(&spec);
    const char *xx2_note=lt48_pic18_xx2_adapter_note(&spec);
    if(xx2_note)return xx2_note;
    const char *xx31_note=lt48_xx31_adapter_note(&spec);
    if(xx31_note)return xx31_note;
    const char *f6_note=lt48_f6_adapter_note(&spec);
    if(f6_note)return f6_note;
    const char *f6x20_note=lt48_f6x20_adapter_note(&spec);
    if(f6x20_note)return f6x20_note;
    const char *x220_note=lt48_x220_adapter_note(&spec);
    const char *otp_note=lt48_pic_otp_adapter_note_internal(installation,device_index);
    if(otp_note)return otp_note;
    for (size_t i = 0; i < sizeof(lt48_pic_rotated_adapters) /
                            sizeof(lt48_pic_rotated_adapters[0]); ++i) {
        const size_t length = strlen(lt48_pic_rotated_adapters[i].name);
        if (spec.catalogue.device_class == lt48_pic_rotated_adapters[i].device_class &&
            strcmp(spec.definition_stem, lt48_pic_rotated_adapters[i].stem) == 0 &&
            spec.catalogue.device_name != NULL &&
            spec.catalogue.device_name_length == length &&
            memcmp(spec.catalogue.device_name,
                   lt48_pic_rotated_adapters[i].name, length) == 0 &&
            spec.definition_resource == lt48_pic_rotated_adapters[i].definition &&
            spec.family_code == lt48_pic_rotated_adapters[i].family &&
            spec.pinmap == lt48_pic_rotated_adapters[i].pinmap) matched = i;
    }
    if (matched == SIZE_MAX && !baseline_note && !x220_note) return NULL;
    if (snprintf(path, sizeof(path), "%s/%s.rez", installation->path,
                 spec.definition_stem) >= (int)sizeof(path)) return NULL;
    if (lt48_rez_open(path, &rez) != LT48_OK) return NULL;
    lt48_status status = lt48_rez_sha256(rez, hash);
    lt48_rez_close(rez);
    if (status != LT48_OK) return NULL;
    for (size_t i = 0; i < sizeof(hash); ++i)
        snprintf(hex + 2 * i, 3, "%02x", hash[i]);
    const char *expected=x220_note?"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640":
        baseline_note?LT48_PIC_BASELINE12_ADAPTER_SHA256:lt48_pic_rotated_adapters[matched].source_sha256;
    if (strcmp(hex, expected) != 0)
        return NULL;
    return x220_note?x220_note:baseline_note?baseline_note:lt48_pic_rotated_adapters[matched].note;
}

lt48_status lt48_installation_device_compile_evidence(
    const lt48_installation *installation, size_t device_index,
    lt48_device_compile_evidence *out_evidence
) {
    lt48_device_spec spec;
    lt48_rez *rez = NULL;
    lt48_family_info family;
    lt48_pin_code pin_code;
    lt48_pin_map *pin_map = NULL;
    lt48_compile_variable *variables = NULL;
    char *path = NULL;
    char name[16];
    size_t family_index;
    size_t resource_index;
    lt48_status status;
    if (installation == NULL || out_evidence == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_evidence, 0, sizeof(*out_evidence));
    status = lt48_installation_resolve_device(
        installation, device_index, &spec
    );
    if (status != LT48_OK) return status;
    if (strlen(installation->path) > SIZE_MAX - strlen(spec.definition_stem) -
        6u) return LT48_INVALID_ARGUMENT;
    path = malloc(strlen(installation->path) +
                  strlen(spec.definition_stem) + 6u);
    if (path == NULL) return LT48_NO_MEMORY;
    sprintf(path, "%s/%s.rez", installation->path, spec.definition_stem);
    status = lt48_rez_open(path, &rez);
    free(path);
    if (status != LT48_OK) return status;
    snprintf(name, sizeof(name), "M%x", spec.family_code);
    status = lt48_rez_find_resource(rez, name, &family_index);
    if (status == LT48_OK)
        status = lt48_rez_get_family_info(rez, family_index, &family);
    snprintf(name, sizeof(name), "P%x", spec.pinmap);
    if (status == LT48_OK)
        status = lt48_rez_find_resource(rez, name, &resource_index);
    if (status == LT48_OK)
        status = lt48_rez_get_pin_code(rez, resource_index, &pin_code);
    if (status == LT48_OK) status = lt48_pin_map_parse(&pin_code, &pin_map);
    if (status != LT48_OK) goto done;
    variables = calloc(family.variable_count ? family.variable_count : 1u,
                       sizeof(*variables));
    if (variables == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < family.variable_count; ++index) {
        lt48_family_variable source;
        status = lt48_rez_get_family_variable(
            rez, family_index, index, &source
        );
        if (status != LT48_OK) goto done;
        variables[index] = (lt48_compile_variable){
            source.values[0], source.values[1], source.values[2],
            source.values[3], source.values[4]
        };
    }
    out_evidence->operation_count = family.operation_count;
    for (size_t index = 0u; index < family.operation_count; ++index) {
        lt48_family_operation operation;
        lt48_wave_code wave_code;
        lt48_compact_wave *wave = NULL;
        lt48_compiled_wave *compiled = NULL;
        status = lt48_rez_get_family_operation(
            rez, family_index, index, &operation
        );
        if (status != LT48_OK) goto done;
        snprintf(name, sizeof(name), "W%x", operation.wave);
        status = lt48_rez_find_resource(rez, name, &resource_index);
        if (status == LT48_OK)
            status = lt48_rez_get_wave_code(rez, resource_index, &wave_code);
        if (status != LT48_OK) goto done;
        if (wave_code.wave_type != 0u) {
            ++out_evidence->local_resource_count;
            continue;
        }
        status = lt48_compact_decode(
            wave_code.data, wave_code.data_size, &wave
        );
        if (status == LT48_OK)
            status = lt48_compact_compile_with_pin_policy(
                wave, pin_map, variables, family.variable_count, 1u,
                LT48_PIN_FALLBACK_STRICT, &compiled
            );
        if (status == LT48_OK) {
            ++out_evidence->strict_compact_count;
        } else {
            lt48_compiled_wave_close(compiled);
            compiled = NULL;
            status = lt48_compact_compile_with_pin_policy(
                wave, pin_map, variables, family.variable_count, 1u,
                LT48_PIN_FALLBACK_RESIDENT_NOOP, &compiled
            );
            if (status == LT48_OK) {
                ++out_evidence->resident_noop_default_count;
            } else {
                lt48_compiled_wave_close(compiled);
                compiled = NULL;
                status = lt48_compact_compile_with_pin_policy(
                    wave, pin_map, variables, family.variable_count, 1u,
                    LT48_PIN_FALLBACK_ANALYSIS_ALL, &compiled
                );
                if (status == LT48_OK)
                    ++out_evidence->unresolved_default_count;
                else
                    ++out_evidence->rejected_compact_count;
            }
        }
        lt48_compiled_wave_close(compiled);
        lt48_compact_wave_close(wave);
        status = LT48_OK;
    }
done:
    free(variables);
    lt48_pin_map_close(pin_map);
    lt48_rez_close(rez);
    return status;
}

static void operation_support_attach_binding(
    lt48_operation_support *support,
    const lt48_device_operation_binding *binding
) {
    support->definition_callback_present = 1;
    support->definition_wave = binding->wave;
    memcpy(support->definition_callback, binding->symbol,
           strlen(binding->symbol) + 1u);
}

static int operation_binding_symbol_is(
    const lt48_device_operation_binding *binding, const char *symbol
) {
    return binding->has_symbol && strcmp(binding->symbol, symbol) == 0;
}

static void operation_support_add_definition_evidence(
    lt48_operation_support *items,
    const lt48_device_operation_binding *bindings,
    size_t binding_count
) {
    for (size_t index = 0u; index < binding_count; ++index) {
        const lt48_device_operation_binding *binding = &bindings[index];
        if (operation_binding_symbol_is(binding, "_FUN_LOAD") ||
            operation_binding_symbol_is(binding, "_FUN_READ") ||
            operation_binding_symbol_is(binding, "_FUN_DNLDREAD") ||
            operation_binding_symbol_is(binding, "_FUN_DnLdRead")) {
            operation_support_attach_binding(&items[0], binding);
            operation_support_attach_binding(&items[1], binding);
            operation_support_attach_binding(&items[4], binding);
            operation_support_attach_binding(&items[5], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_BLKCHK") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDBLKCHK") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DnLd_BLKCHK")) {
            operation_support_attach_binding(&items[1], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_VERIFY") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDVERIFY") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DnLd_VERIFY")) {
            operation_support_attach_binding(&items[4], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_PROGRAM") ||
                   operation_binding_symbol_is(binding, "_FUN_PROG") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_PROGRAMWORD") ||
                   operation_binding_symbol_is(binding, "_FUN_DNLDPROG") ||
                   operation_binding_symbol_is(binding, "_FUN_DnLdProg") ||
                   operation_binding_symbol_is(binding, "_FUN_DnLd_PROG") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_ProgramPLine") ||
                   operation_binding_symbol_is(binding, "_FUN_POSTPROG")) {
            operation_support_attach_binding(&items[2], binding);
            operation_support_attach_binding(&items[3], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_ERASE") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDERASE") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDPREERASE")) {
            operation_support_attach_binding(&items[6], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_PROTECT") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDPROTECT") ||
                   operation_binding_symbol_is(binding, "_FUN_UNPROTECT") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDUNPROTECT") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_PREUNPROTECT") ||
                   operation_binding_symbol_is(binding, "_FUN_UNLOCK")) {
            operation_support_attach_binding(&items[8], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_SECU") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_ProgEncrypt")) {
            operation_support_attach_binding(&items[7], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_CheckID") ||
                   operation_binding_symbol_is(binding, "_FUN_GETID") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDGETID")) {
            operation_support_attach_binding(&items[11], binding);
        } else if (operation_binding_symbol_is(binding, "_FUN_LoadConfig") ||
                   operation_binding_symbol_is(binding, "_FUN_LOADconfig") ||
                   operation_binding_symbol_is(binding, "_FUN_PROGRAMconfig") ||
                   operation_binding_symbol_is(binding, "_FUN_ProgramConf") ||
                   operation_binding_symbol_is(binding, "_FUN_ReadConf") ||
                   operation_binding_symbol_is(binding, "_FUN_READCFG") ||
                   operation_binding_symbol_is(binding,
                                               "_FUN_DNLDREADCFG") ||
                   operation_binding_symbol_is(binding, "_FUN_PreConfig") ||
                   operation_binding_symbol_is(binding, "_FUN_PostConfig")) {
            if (!items[9].definition_callback_present)
                operation_support_attach_binding(&items[9], binding);
        }
    }
    if (items[0].definition_callback_present) {
        items[0].reason =
            "definition has an exact load/read callback, but this engine has no validated native Read workflow";
        items[1].reason =
            "definition has the acquisition callback needed by Blank Check, but its host workflow and result protocol are not validated";
        items[4].reason =
            "definition has the acquisition callback needed by Verify, but its host workflow and result protocol are not validated";
        items[5].reason =
            "definition has the acquisition callback needed by Compare, but its host workflow and mismatch handling are not validated";
    }
    if (items[2].definition_callback_present) {
        items[2].reason =
            "definition has an exact program callback, but native planning and execution for this engine are not validated";
        items[3].reason =
            "definition has an exact program callback, but over-program transition rules and execution are not validated";
    }
    if (items[6].definition_callback_present)
        items[6].reason =
            "definition has an exact erase callback, but its region, timing, polling, protection and cleanup semantics are not validated";
    if (items[7].definition_callback_present)
        items[7].reason =
            "definition has an exact secure/encrypt callback, but its irreversible state model, result protocol and cleanup semantics are not decoded";
    if (items[8].definition_callback_present)
        items[8].reason =
            "definition has an exact protection callback, but protection semantics and reversibility are not decoded";
    if (items[9].definition_callback_present)
        items[9].reason =
            "definition has configuration callbacks, but typed regions and the application workflow are not decoded";
    if (items[11].definition_callback_present)
        items[11].reason =
            "definition has an exact device-ID callback, but its electrical recipe and result decoder are not validated";
}

static lt48_status operation_support_collect_definition_evidence(
    const lt48_installation *installation,
    size_t device_index,
    lt48_operation_support *items
) {
    lt48_device_operation_binding *bindings = NULL;
    size_t binding_count = 0u;
    lt48_status status = lt48_installation_device_operations(
        installation, device_index, NULL, 0u, &binding_count);
    if (status == LT48_NOT_FOUND ||
        (status == LT48_BUFFER_TOO_SMALL && binding_count == 0u))
        return LT48_OK;
    if (status != LT48_BUFFER_TOO_SMALL) return status;
    bindings = calloc(binding_count, sizeof(*bindings));
    if (bindings == NULL) return LT48_NO_MEMORY;
    status = lt48_installation_device_operations(
        installation, device_index, bindings, binding_count, &binding_count);
    if (status == LT48_OK)
        operation_support_add_definition_evidence(
            items, bindings, binding_count);
    free(bindings);
    return status;
}

lt48_status lt48_installation_operation_support(
    const lt48_installation *installation, size_t device_index,
    lt48_operation_support *items, size_t capacity, size_t *out_count
) {
#define BASELINE_SUPPORT(kind_, name_, reason_, mutates_) \
    { .operation = (kind_), .name = (name_), \
      .availability = LT48_OPERATION_UNAVAILABLE, \
      .support_level = "unavailable", .reason = (reason_), \
      .mutates_device = (mutates_) }
    static const lt48_operation_support baseline[LT48_OPERATION_KIND_COUNT] = {
        BASELINE_SUPPORT(LT48_OPERATION_READ, "read",
            "no validated native engine for this definition", 0),
        BASELINE_SUPPORT(LT48_OPERATION_BLANK_CHECK, "blank-check",
            "no validated native engine for this definition", 0),
        BASELINE_SUPPORT(LT48_OPERATION_PROGRAM, "program",
            "native program planning is not implemented", 1),
        BASELINE_SUPPORT(LT48_OPERATION_OVER_PROGRAM, "over-program",
            "read/transition proof and native program execution are not implemented", 1),
        BASELINE_SUPPORT(LT48_OPERATION_VERIFY, "verify",
            "no validated native engine for this definition", 0),
        BASELINE_SUPPORT(LT48_OPERATION_COMPARE, "compare",
            "no validated native Compare engine for this definition; retained-image comparison is a separate Buffer command", 0),
        BASELINE_SUPPORT(LT48_OPERATION_ERASE, "erase",
            "native erase planning is not implemented", 1),
        BASELINE_SUPPORT(LT48_OPERATION_SECURE, "secure",
            "protection semantics and reversibility are not decoded", 1),
        BASELINE_SUPPORT(LT48_OPERATION_MEMORY_PROTECT, "memory-protect",
            "protection semantics and reversibility are not decoded", 1),
        BASELINE_SUPPORT(LT48_OPERATION_CONFIGURATION, "configuration",
            "configuration schema and application workflow are not decoded", 1),
        BASELINE_SUPPORT(LT48_OPERATION_FUNCTIONAL_TEST, "functional-test",
            "functional-vector execution is not implemented", 0),
        BASELINE_SUPPORT(LT48_OPERATION_AUTO_ID, "auto-id",
            "global six-recipe EPROM provider planning and offline ranking are available outside selected-device operations; guarded live acquisition is not integrated", 0),
        BASELINE_SUPPORT(LT48_OPERATION_MASS_PRODUCTION, "mass-production",
            "requires an executable validated program workflow", 1),
        BASELINE_SUPPORT(LT48_OPERATION_JTAG_ISP, "jtag-isp",
            "external-target TAP and adapter validation are not implemented", 1)
    };
#undef BASELINE_SUPPORT
    lt48_catalogue_entry entry;
    lt48_m37_resources *resources = NULL;
    lt48_palce_resources *palce_resources = NULL;
    lt48_gal_resources *gal_resources = NULL;
    lt48_m37_program_plan_set *program_set = NULL;
    const lt48_m37_resource_audit *audit;
    lt48_protocol_plan *plan = NULL;
    unsigned char verify_byte[2] = {0xffu,0xffu};
    uint32_t probe_length=1;
    lt48_status status;

    if (installation == NULL || out_count == NULL ||
        (items == NULL && capacity != 0u)) return LT48_INVALID_ARGUMENT;
    *out_count = LT48_OPERATION_KIND_COUNT;
    status = lt48_installation_get_device(installation, device_index, &entry);
    if (status != LT48_OK) return status;
    if (items == NULL || capacity < LT48_OPERATION_KIND_COUNT)
        return LT48_BUFFER_TOO_SMALL;
    memcpy(items, baseline, sizeof(baseline));
    status = operation_support_collect_definition_evidence(
        installation, device_index, items);
    if (status != LT48_OK) return status;
    if(entry.device_class==2048u){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sy76_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sy76_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if(entry.device_class==2038u){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_mw36_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mw36_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sm30_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sm30_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sx30_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sx30_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_mg15_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mg15_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sn9_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sn9_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_ob16_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_ob16_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2035u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_so30_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_so30_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_wt10_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_wt10_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_wt8_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_wt8_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_hy14_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_hy14_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_mw6_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mw6_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_my7_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_my7_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_my4_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_my4_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sc2_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sc2_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sc4g_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sc4g_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sc4a_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sc4a_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_tp4_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_tp4_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_tp6_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_tp6_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_mw8_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mw8_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_gc8_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_gc8_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2039u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_ab11_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_ab11_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_su16_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_su16_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sm3_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sm3_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(installation,device_index,&z)&&lt48_sm6_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sm6_admitted_internal(entry.device_class,entry.device_type))return LT48_OK;}
    if(lt48_tpi15_binding_internal(&entry)){
       lt48_xp_flash_resources*r=NULL;if(lt48_tpi15_admitted_internal()&&!lt48_xp_flash_open(installation,device_index,&r))for(size_t i=0;i<7;i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="tpi15_source_audited";items[i].reason="Original TPI code operations including raw Read; fuse preservation, protection refusal, original adapter; hardware pending";}
       lt48_xp_flash_close(r);return LT48_OK;
    }
    if(lt48_xp13_binding_internal(&entry)){
      lt48_xp_flash_resources*r=NULL;
      if(lt48_xp13_admitted_internal()&&lt48_xp_flash_open(installation,device_index,&r)==LT48_OK)
       for(size_t i=0;i<(entry.device_class==2035?6u:7u);i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="xp13_source_audited";items[i].reason=entry.device_class==2035?"Original low-voltage word OTP/EPROM; raw Read; electrical Erase and Secure unavailable; original adapter; hardware pending":"Exact-resource Sharp-compatible NOR; source geometry, raw Read and original operations; hardware pending";}
      lt48_xp_flash_close(r);return LT48_OK;
    }
    if(lt48_lpc16_member_internal(entry.device_class,entry.device_type)||lt48_nxp18_member_internal(entry.device_class,entry.device_type)){
        lt48_xp_flash_resources*r=NULL;
        if((lt48_lpc16_admitted_internal(entry.device_type)||lt48_nxp18_admitted_internal(entry.device_type))&&lt48_xp_flash_open(installation,device_index,&r)==LT48_OK)
            for(size_t i=0;i<7;i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=tmp1962one_member(&entry)?"tmp1962one_source_audited":lt48_lpc16_member_internal(entry.device_class,entry.device_type)?"lpc16_source_audited":"nxp18_source_audited";items[i].reason="Original LPC complete-image flash workflow, raw Read and session initialization; CRP/NO_ISP programming unavailable; hardware pending";}
        lt48_xp_flash_close(r);return LT48_OK;
    }
    if (entry.device_class == 2020u || entry.device_class == 2024u || entry.device_class == 2023u) {
        if(entry.device_class==2023u){lt48_device_spec candidate;if(lt48_installation_resolve_device(installation,device_index,&candidate)!=LT48_OK||!(palce54_binding(&candidate)||palce8_binding(&candidate)||mach11_binding(&candidate)))goto legacy_palce_support;}
        lt48_device_spec spec;
        char *path = NULL;
        int count;
        status = lt48_installation_resolve_device(
            installation, device_index, &spec);
        if (status != LT48_OK || spec.definition_stem == NULL)
            return LT48_OK;
        count = snprintf(NULL, 0, "%s/%s.rez", installation->path,
                         spec.definition_stem);
        if (count < 0) return LT48_OK;
        path = malloc((size_t)count + 1u);
        if (path == NULL) return LT48_OK;
        (void)snprintf(path, (size_t)count + 1u, "%s/%s.rez",
                       installation->path, spec.definition_stem);
        status = lt48_gal_resources_open_definition(
            path, spec.definition_resource, spec.family_code, spec.pinmap,
            &gal_resources);
        free(path);
        if (status != LT48_OK) return LT48_OK;
        /* Admit only the exact hardware-qualified definition. Shared family
         * resources alone are not sufficient to enable other GAL variants. */
        static const lt48_operation_kind gal_operations[] = {
            LT48_OPERATION_READ, LT48_OPERATION_BLANK_CHECK,
            LT48_OPERATION_PROGRAM, LT48_OPERATION_VERIFY,
            LT48_OPERATION_ERASE, LT48_OPERATION_SECURE
        };
        for (size_t index = 0u; index < sizeof(gal_operations) /
                                    sizeof(gal_operations[0]); ++index) {
            lt48_operation_support *support =
                &items[gal_operations[index] - 1u];
            support->availability = LT48_OPERATION_PLAN_ONLY;
            support->support_level = "native_gal_resource_plan_only";
            support->reason =
                "digest-locked GBA family/pin/PES resources are loaded and "
                "compiled; GAL transport and physical qualification are still required";
            if (lt48_gal_resources_source_audited(gal_resources)) {
                support->availability = LT48_OPERATION_EXECUTABLE;
                support->support_level = ejed9_binding(&spec)?"ejed9_source_audited":mach11_binding(&spec)?"mach11_source_audited":palce8_binding(&spec)?"palce8_source_audited":palce54_binding(&spec) ? "palce54_source_audited" : cpld69_binding(&spec) ? "cpld69_source_audited" : gal59_binding(&spec) ? "gal59_source_audited" : "ns_gal_source_audited";
                support->reason = ejed9_binding(&spec)?"Audited MACH/M4 main and E-field operations including raw Read; original adapters required, hardware pending":mach11_binding(&spec)?"Exact MACH row buffering, source fuse maps and six operations including raw Read; original package adapter required, hardware pending":palce8_binding(&spec)?"Exact PALCE610/630 source word tables and bitmaps, complete six operations and raw Read; hardware pending":palce54_binding(&spec) ? "digest-bound PALCE/ATF Read, Blank, Program, Verify, Erase and Secure; original compact procedures and fuse maps; hardware testing pending" : cpld69_binding(&spec) ? "digest-bound CPLD Read, Blank, Program, Verify, Erase and Secure; original compact procedures, main/E/User fields and verification passes; hardware testing pending" : gal59_binding(&spec) ? "digest-bound GAL/CPLD Read, Blank, Program, Verify, Erase and Secure; original setup, PES and fuse procedures; hardware testing pending" : "digest-bound National GAL Read, Blank, Program, Verify, Erase and Secure; original PES and fuse procedures; hardware testing pending";
            }
            if (lt48_gal_resources_live_qualified(gal_resources)) {
                support->availability = LT48_OPERATION_EXECUTABLE;
                support->support_level = "native_gal_hardware_verified";
                support->reason = "source-locked D19/M18/P3 C lifecycle passed read, blank, program, verify, erase and Secure; erase/reprogram restored exact hardware readback";
            }
        }
        lt48_gal_resources_close(gal_resources);
        return LT48_OK;
    }
    if(entry.device_class==2017u) {
        lt48_xc7000_resources *r=NULL;
        if(lt48_xc7000_open(installation,device_index,&r)==LT48_OK) {
            const lt48_operation_kind operations[]={LT48_OPERATION_READ,LT48_OPERATION_BLANK_CHECK,LT48_OPERATION_PROGRAM,LT48_OPERATION_OVER_PROGRAM,LT48_OPERATION_VERIFY,LT48_OPERATION_COMPARE,LT48_OPERATION_SECURE};
            for(size_t i=0;i<sizeof operations/sizeof *operations;i++) {
                lt48_operation_support *item=&items[operations[i]-1];
                item->availability=LT48_OPERATION_EXECUTABLE;item->support_level="xc7000_source_audited";
                item->reason="Digest-bound XC72xx/XC73xx sparse rows, masked verification and bounded retries; source audited, hardware testing pending; source package adapter required";
            }
            items[LT48_OPERATION_ERASE-1].reason="No electrical erase procedure in this source; windowed parts require external UV erase";
            lt48_xc7000_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2026u||entry.device_class==2028u) {
        lt48_pof_device *pof=NULL;
        lt48_status opened=lt48_pof_device_open(installation,device_index,&pof);
        if(opened==LT48_OK) {
            lt48_status runtime=lt48_pof_device_runtime_status(pof);
            const lt48_operation_kind operations[]={LT48_OPERATION_READ,LT48_OPERATION_BLANK_CHECK,
                LT48_OPERATION_PROGRAM,LT48_OPERATION_OVER_PROGRAM,LT48_OPERATION_VERIFY,
                LT48_OPERATION_COMPARE,LT48_OPERATION_SECURE,LT48_OPERATION_FUNCTIONAL_TEST};
            for(size_t i=0;i<sizeof operations/sizeof *operations;i++) {
                lt48_operation_support *item=&items[operations[i]-1];
                item->availability=LT48_OPERATION_EXECUTABLE;
                item->support_level="pof_source_audited";
                item->reason=runtime==LT48_OK?
                    "Digest-bound GC/GCB POF image and functional-vector operations; source audit, hardware testing pending":
                    "Import the original Dataman 13.10 runtime for the selected POF procedure";
            }
            items[LT48_OPERATION_ERASE-1].reason="This OTP logic procedure has no electrical erase; use a blank device for programming";
            lt48_pof_device_close(pof);return LT48_OK;
        }
        lt48_pof_device_close(pof);
        /* Other source versions remain unqualified; their catalogue must
         * still export. Only the exact facade match admits this workflow. */
        if(opened==LT48_NO_MEMORY)return opened;
    }
    if (entry.device_class == 2019u || entry.device_class == 2022u || entry.device_class == 2021u || entry.device_class == 2027u || entry.device_class == 2025u) {
        lt48_device_spec spec;
        lt48_pal_resources *pal_resources = NULL;
        char *path = NULL;
        int count = 0;
        status = lt48_installation_resolve_device(
            installation, device_index, &spec);
        if (status != LT48_OK || spec.definition_stem == NULL)
            return LT48_OK;
        count = snprintf(NULL, 0, "%s/%s.rez", installation->path,
                         spec.definition_stem);
        if (count < 0) return LT48_OK;
        path = malloc((size_t)count + 1u);
        if (path == NULL) return LT48_OK;
        (void)snprintf(path, (size_t)count + 1u, "%s/%s.rez",
                       installation->path, spec.definition_stem);
        status = lt48_pal_resources_open_definition(
            path, spec.definition_resource, spec.family_code, spec.pinmap,
            &pal_resources);
        free(path);
        if (status != LT48_OK) return LT48_OK;
        /* GBB pin-control matrices and the first native session are implemented.
         * Complete source/transport qualification remains a separate boundary. */
        uint16_t selected_erase=0,selected_secure=0;
        int has_erase=lt48_pal_resources_operation_internal(pal_resources,"_FUN_ERASE",&selected_erase)==LT48_OK;
        int has_secure=lt48_pal_resources_operation_internal(pal_resources,"_FUN_SECU",&selected_secure)==LT48_OK;
        static const lt48_operation_kind pal_operations[] = {
            LT48_OPERATION_READ, LT48_OPERATION_BLANK_CHECK,
            LT48_OPERATION_PROGRAM, LT48_OPERATION_VERIFY, LT48_OPERATION_SECURE
        };
        for (size_t index = 0u; index < sizeof(pal_operations) /
                                    sizeof(pal_operations[0]); ++index) {
            lt48_operation_support *support =
                &items[pal_operations[index] - 1u];
            if (pal_operations[index] == LT48_OPERATION_SECURE &&
                !has_secure) continue;
            support->availability = LT48_OPERATION_PLAN_ONLY;
            support->support_level = "native_pal_resource_plan_only";
            support->reason =
                "digest-locked GBB type-0 resources are loaded and compiled; "
                "complete procedure and live transport qualification are still required";
        }
        int pal46_new = !strcmp(spec.definition_stem,"GBB____") &&
            (pal46_definition(spec.definition_resource,spec.family_code,spec.pinmap)||pal29_definition(spec.definition_resource,spec.family_code,spec.pinmap)||ep35_stage(spec.definition_resource,spec.family_code,spec.pinmap)||pal52_stage(spec.definition_resource,spec.family_code,spec.pinmap));
        int epm44_new=entry.device_class==2027u && epm44_stage(spec.definition_resource,spec.family_code,spec.pinmap)!=0;
        if (lt48_pal_resources_source_audited(pal_resources) && (!epm44_new || epm44_binding(&spec)) &&
            (!pal46_new || pal46_binding(&spec)||pal29_binding(&spec)||ep35_binding(&spec)||pal52_binding(&spec)) && (entry.device_class!=2019u || peel46_binding(&spec)||pz20_binding(&spec)) && (entry.device_class!=2022u || compact20_binding(&spec)||pls26_binding(&spec)||epx6_binding(&spec))) {
            for (size_t i = 0; i < sizeof(pal_operations)/sizeof(pal_operations[0]); ++i) {
                lt48_operation_support *support = &items[pal_operations[i] - 1u];
                if(pal_operations[i]==LT48_OPERATION_SECURE && !has_secure)continue;
                support->availability = LT48_OPERATION_EXECUTABLE;
                support->support_level = pz20_binding(&spec)?"pz20_source_audited":epx6_binding(&spec)?"epx6_source_audited":pls26_binding(&spec)?"pls26_source_audited":compact20_binding(&spec)?"compact20_source_audited":peel46_binding(&spec)?(lt48_pal_resources_security_marker(pal_resources,&(size_t){0})==LT48_OK?"peel46_security_corrected_audited":"peel46_source_audited"):pal52_binding(&spec)?"pal52_source_audited":ep22four_binding(&spec)?"ep22four_source_audited":ep35_binding(&spec)?"ep35_source_audited":pal29_binding(&spec) ? "pal29_source_audited" : entry.device_class == 2027u ? (epm44_new?"epm44_source_audited":"epm_source_audited") :
                    lt48_pal_resources_authored_corrections(pal_resources) ? "pal_authored_mmi_audited" : "pal_source_audited";
                support->reason = pz20_binding(&spec)?"Exact PEEL/PZ/XCR source word tables and bitmaps, complete C and GUI operations including raw Read; hardware pending":epx6_binding(&spec)?"Exact EPX source bitmap ordering, raw Read and original compact operations; hardware pending":pls26_binding(&spec)?"Exact Philips/TI compact PLD source tables, raw Read and fuse layouts; source-selected operations only; hardware pending":compact20_binding(&spec)?"original compact PLD/clock workflows, raw Read and complete source-selected operations; hardware pending":peel46_binding(&spec)?"original compact PEEL/Atmel/Philips PLD workflows, raw Read and source-selected operations; hardware pending":pal52_binding(&spec)?"original PAL/EPLD raw Read, Blank, Program, Verify and Secure; source pin-control tables and fuse maps; hardware pending":ep35_binding(&spec) ? "original EPLD Read, Blank, Program, Verify and Secure; factory-selected physical map and additional raw configuration comparison; hardware testing pending" : lt48_pal_resources_authored_corrections(pal_resources) ?
                    "MMI-documented authored fuse geometry/polarity corrections; complete C/GUI OTP PAL operations; hardware testing pending" :
                    "digest-bound OTP PAL Read, Blank, Program, Verify and Secure; original host row comparison and cleanup; hardware testing pending";
            }
        }
        if (has_erase) {
            lt48_operation_support *support =
                &items[LT48_OPERATION_ERASE - 1u];
            support->availability = LT48_OPERATION_PLAN_ONLY;
            support->support_level = "native_pal_resource_plan_only";
            support->reason =
                "this GBB definition has a compiled erase operation; "
                "complete procedure and live transport qualification are still required";
        }
        if((peel46_binding(&spec)||pz20_binding(&spec)) && has_erase){
            items[LT48_OPERATION_ERASE-1].availability=LT48_OPERATION_EXECUTABLE;
            items[LT48_OPERATION_ERASE-1].support_level=pz20_binding(&spec)?"pz20_source_audited":"peel46_source_audited";
            items[LT48_OPERATION_ERASE-1].reason="source-selected Erase followed by full blank check; hardware pending";
        }
        if(pal52_binding(&spec) && lt48_pal_resources_has_operation(pal_resources,6u)) {
            items[LT48_OPERATION_ERASE-1].availability=LT48_OPERATION_EXECUTABLE;
            items[LT48_OPERATION_ERASE-1].support_level="pal52_source_audited";
            items[LT48_OPERATION_ERASE-1].reason="source-defined electrical erase followed by blank check; hardware qualification pending";
        }
        if(entry.device_class == 2027u && lt48_pal_resources_source_audited(pal_resources) && (!epm44_new || epm44_binding(&spec))) {
            items[LT48_OPERATION_ERASE-1].availability=LT48_OPERATION_EXECUTABLE;
            items[LT48_OPERATION_ERASE-1].support_level=epm44_new?"epm44_source_audited":"epm_source_audited";
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)
                if(items[i].availability==LT48_OPERATION_EXECUTABLE)
                    items[i].reason="digest-bound Altera EPM POF fuse operations, factory timing decode and cleanup; source audit, hardware testing pending";
        }
        if(entry.device_class==2025u && lt48_pal_resources_source_audited(pal_resources)) {
            for(unsigned i=0;i<8;i++) {
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level="xc95_source_audited";
                items[i].reason="digest-bound XC9500 fuse operations, bounded program/erase retries, verified protection and cleanup; hardware testing pending";
            }
        }
        lt48_pal_resources_close(pal_resources);
        return LT48_OK;
    }
legacy_palce_support:
    if (entry.device_class == 2023u) {
        lt48_device_spec spec;
        char *path = NULL;
        int count = 0;
        status = lt48_installation_resolve_device(
            installation, device_index, &spec);
        if (status != LT48_OK || spec.definition_stem == NULL)
            return LT48_OK;
        count = snprintf(NULL, 0, "%s/%s.rez", installation->path,
                         spec.definition_stem);
        if (count < 0) return LT48_OK;
        path = malloc((size_t)count + 1u);
        if (path == NULL) return LT48_OK;
        (void)snprintf(path, (size_t)count + 1u, "%s/%s.rez",
                       installation->path, spec.definition_stem);
        status = lt48_palce_resources_open_definition(
            path, spec.definition_resource, &palce_resources);
        free(path);
        if (status != LT48_OK) return LT48_OK;
        /* Exact Dataman D0/D2 procedures: D0 has a full physical security
         * cycle; D2 security reuses the audited worker/programming path with
         * its own 18-byte security-row waveform (source evidence). */
        static const lt48_operation_kind palce_operations[] = {
            LT48_OPERATION_READ, LT48_OPERATION_BLANK_CHECK,
            LT48_OPERATION_PROGRAM, LT48_OPERATION_VERIFY,
            LT48_OPERATION_ERASE, LT48_OPERATION_SECURE
        };
        for (size_t index = 0u; index < sizeof(palce_operations) /
                                    sizeof(palce_operations[0]); ++index) {
            lt48_operation_support *support =
                &items[palce_operations[index] - 1u];
            support->availability = LT48_OPERATION_PLAN_ONLY;
            support->support_level = "native_owner_plan_only";
            support->reason =
                "digest-locked selected PALCE resources and C owner are available; "
                "physical C transport qualification is still required before live execution";
            if (((spec.definition_resource == 1u && spec.family_code == 1u && spec.pinmap == 1u) ||
                 (spec.definition_resource == 2u && spec.family_code == 2u && spec.pinmap == 4u) ||
                 (spec.definition_resource == 0u && spec.family_code == 0u && spec.pinmap == 0u)) && !strcmp(
                    lt48_palce_resources_source_version(palce_resources),
                    "Dataman 13.10")) {
                support->availability = LT48_OPERATION_EXECUTABLE;
                support->support_level = spec.definition_resource == 1u ? "palce20_source_audited" : spec.definition_resource == 0u ?
                    "native_palce16_secure_hardware_verified" :
                    (palce_operations[index] == LT48_OPERATION_SECURE ?
                     "palce22_secure_source_audited" : "native_palce22_hardware_verified");
                support->reason = spec.definition_resource == 1u ?
                    "Exact D1/M1/P1, 2706 fuses; original read/program/erase/security waves, ID-dependent voltage and nested erase preserved; hardware testing pending" : spec.definition_resource == 0u ?
                    "digest-locked D0/M0/P0 full native program/verify/secure/erase/reprogram "
                    "cycle passed; catalogue aliases inherit the exact source procedure" :
                    (palce_operations[index] == LT48_OPERATION_SECURE ?
                     "original mode8 Secure worker and D2 security-row waveform audited; "
                     "uses the hardware-qualified D2 programming engine; physical Secure test was D0 only" :
                     "digest-locked D2/M2/P4 C read/blank/program/verify/erase passed hardware readback");
            }
        }
        lt48_palce_resources_close(palce_resources);
        return LT48_OK;
    }
    if(entry.device_class==2010u && (entry.device_type==0xfffe||entry.device_type==0x91)){
        lt48_stk10c68_resources *r=NULL;
        if(lt48_stk10c68_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<7;i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type==0x91?"stk11one_source_audited":"stk10c68_hardware_qualified";items[i].reason=entry.device_type==0x91?"Source-audited STK11C68 software STORE/RECALL, independent persistence checks, seven C/GUI operations; hardware pending":"Native STK10C68-C45 extension; hardware STORE/RECALL, preserved partial writes, full persistence verification at 5 V";}
            lt48_stk10c68_close(r);
        }
        return LT48_OK;
    }
    if(entry.device_class==2000u){
        lt48_compact_spi_resources *r=NULL;
        if(lt48_compact_spi_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<(lt48_compact_spi_has_protection(r)?8u:7u);i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="compact_spi_source_audited";items[i].reason="Exact selected compact SPI procedure; nonblank rewrites, preserved partial chunks, margin and fresh-power verification; clear writes FF";}
            if(lt48_compact_spi_has_raw_status(r)) {
                for(size_t i=0;i<8;i++)items[i].reason="Original SPI flash erase/program waves; full reconstruction for partial writes, raw status preservation and independent verification; hardware qualification pending";
                items[LT48_OPERATION_CONFIGURATION-1].availability=LT48_OPERATION_EXECUTABLE;
                items[LT48_OPERATION_CONFIGURATION-1].support_level="compact_spi_source_audited";
                items[LT48_OPERATION_CONFIGURATION-1].reason="Explicit SRWD/BP setting; preserve array and independently read back raw status";
            }
            if(lt48_compact_spi_has_incremental_registers(r)) {
                for(size_t i=0;i<8;i++)items[i].reason="Original ST35080 byte/WRINC procedures; preserve counter pairs and status; counters cannot decrease or erase; hardware qualification pending";
                items[LT48_OPERATION_ERASE-1].availability=LT48_OPERATION_UNAVAILABLE;
                items[LT48_OPERATION_ERASE-1].reason="No whole-device erase: first32 bytes are increment-only counters; program FF only in ordinary EEPROM addresses32..1023 to clear that region";
                items[LT48_OPERATION_BLANK_CHECK-1].reason="Factory-state check: counters zero at0..31 and ordinary EEPROM FF at32..1023";
            }
            if(lt48_compact_spi_has_idlock(r)) {
                items[LT48_OPERATION_SECURE-1].reason="Explicit IDLock region 1..7; no whole-array lock exists; array preserved and status independently verified";
                items[LT48_OPERATION_CONFIGURATION-1].availability=LT48_OPERATION_EXECUTABLE;
                items[LT48_OPERATION_CONFIGURATION-1].support_level="compact_spi_source_audited";
                items[LT48_OPERATION_CONFIGURATION-1].reason="IDLock selector 0..7, including unlock; original PROTECT waveform and independent status/array readback; hardware qualification pending";
            }
            lt48_compact_spi_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2010u||entry.device_class==2026u){
        lt48_config_memory_resources *r=NULL;
        if(lt48_config_memory_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(lt48_config_memory_mask_internal(r)&(1u<<i)){
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level=LT48_FAMILY_ATT_CONFIG_AUDIT_LEVEL;
                items[i].reason="Digest-bound configuration memory procedures, complete stream and independent readback; hardware qualification pending";
            }
            lt48_config_memory_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2010u){lt48_lpcflash_resources*r=NULL;if(lt48_lpcflash_open(installation,device_index,&r)==LT48_OK){
        for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++){if(items[i].operation==LT48_OPERATION_READ){items[i].availability=LT48_OPERATION_UNAVAILABLE;items[i].reason="Code Read unavailable in original P89LPC interface; use Configuration to read settings";}else if((items[i].operation>=2&&items[i].operation<=8)||items[i].operation==LT48_OPERATION_CONFIGURATION){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="lpc_flash_source_audited";items[i].reason="Original P89LPC operation set; sector CRC verification, page programming, erase, configuration and security; no raw code Read; hardware pending";}}
        lt48_lpcflash_close(r);return LT48_OK;}}
    #include "generated/config_fields_support.inc"


    if(entry.device_class==2018u){
        lt48_ic2_resources *r=NULL;
        if(lt48_ic2_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type==0xeb?"mcu70_source_audited":"at89_ic2_source_audited";
                items[i].reason=entry.device_type==0xeb?"Exact IB2 source resources; full 16-KiB code operations, restored settings after erase, independent margin/configuration readback and failure cleanup; hardware qualification pending":"Exact IC2 source resources; full 32-KiB code operations, restored settings after erase, independent margin/configuration readback and failure cleanup; hardware qualification pending";
            }
            lt48_ic2_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2010u){
        lt48_xc17_resources *r=NULL;
        if(lt48_xc17_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(lt48_xc17_mask_internal(r) & (1u<<i)){
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level=!strcmp(lt48_xc17_profile_internal(r),LT48_FAMILY_XC17_PROFILE)?LT48_FAMILY_XC17_AUDIT_LEVEL:LT48_FAMILY_SERIAL_PROM_AUDIT_LEVEL;
                items[i].reason="Exact original serial PROM procedures, full-array OTP operations, preserved configuration and independent margin readback; hardware qualification pending";
            }
            lt48_xc17_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2010u){
        lt48_avr_resources *r=NULL;
        if(lt48_avr_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<8;i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=(entry.device_type>=0xfff0 && entry.device_type<=0xfff5)?"avr_native_alias":"avr_source_audited";items[i].reason="Digest-bound AVR Flash/EEPROM/fuse/lock workflow; source voltage margins and independent readback";}
            lt48_avr_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2015u){
        lt48_pic_resources *r=NULL;
        if(lt48_pic_open(installation,device_index,&r)==LT48_OK){
            uint32_t mask=lt48_pic_operation_mask_internal(r);
            for(size_t i=0;i<8;i++){if(!(mask&(1u<<(items[i].operation-1)))){items[i].availability=LT48_OPERATION_UNAVAILABLE;items[i].reason="UV/OTP memory has no electrical erase operation";continue;}items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=lt48_pic_is_hms77two_internal(r)?"hms77two_source_audited":lt48_pic_is_hyundai4_internal(r)?"hyundai4_source_audited":lt48_pic_is_pic18c_internal(r)?"pic18c_eprom_source_audited":lt48_pic_is_older17_internal(r)?"pic_older17_source_audited":lt48_pic_is_remaining14_internal(r)?"pic_remaining14_source_audited":lt48_pic_is_baseline12_otp_internal(r)?"pic_baseline12_otp_source_audited":lt48_pic_is_tail14_internal(r)?"pic_tail14_source_audited":lt48_pic_is_calibrated14_internal(r)?"pic_calibrated14_source_audited":mask==0xbfu?"pic_otp14_source_audited":lt48_pic_capacity(r)==0x4014u?"pic_midrange14_source_audited":lt48_pic_capacity(r)==0x10018u?"pic_common_batch_source_audited":lt48_pic_capacity(r)==0x10012u?"pic1704_source_audited":lt48_pic_capacity(r)==8192u?"pic_baseline12_source_audited":"pic_source_audited";items[i].reason="Digest-bound PIC code/data/configuration workflow; full-image programming, source voltage margins, delayed protection and independent readback; see per-device hardware scope";}
            lt48_pic_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2033u)&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_at91nine_admitted_internal()&&!lt48_at91nine_open(installation,device_index,&r)){if(r->a91_width==2||(r->configuration_maximum[1]<=255&&r->capacity<=65536)){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=r->a91_bridge?"at91four_source_audited":"at91nine_source_audited";items[i].reason="AT91 whole-image raw Read; original FFPI workflow; security action write-only; original adapter; hardware pending";}lt48_s8253_close(r);return LT48_OK;}lt48_s8253_close(r);}}
    if((entry.device_class==2038u||entry.device_class==2044u)&&lt48_mf74_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_mf74_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="mf74_source_audited";items[i].reason="Exact Megawin/FUDAN original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_mw27_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_mw27_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="mw27_source_audited";items[i].reason="Exact Megawin XP original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2048u&&lt48_sy80_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sy80_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sy80_source_audited";items[i].reason="Exact SyncMos SM59R original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2048u&&lt48_sy76_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sy76_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sy76_source_audited";items[i].reason="Exact SyncMos SM39/59R and On-Bright original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_mw36_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_mw36_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=(entry.device_type==367)?"mwF6one_source_audited":(entry.device_type==344||entry.device_type==345)?"mwM2_source_audited":"mw36_source_audited";items[i].reason="Exact Megawin MG82/MG87 original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_sm30_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sm30_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sm30_source_audited";items[i].reason="Exact SyncMos SM591x / SM59Dxx original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_sx30_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sx30_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sx30_source_audited";items[i].reason="Exact SyncMos SM595x / SM895x original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_mg15_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_mg15_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="mg15_source_audited";items[i].reason="Exact Magna HMS99C5xS original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_sn9_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sn9_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7)||(!lt48_snas4_array_internal(r->definition)&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=lt48_snas4_array_internal(r->definition)?"snas4_source_audited":r->configuration_id==25113?"cion2_source_audited":"sn9_source_audited";items[i].reason=lt48_snas4_array_internal(r->definition)?"Seven array operations with raw Read; lock state unavailable; Program/Over Program erase entire chip; Secure/configuration unavailable; original adapter; hardware pending":r->configuration_id==25113?"Cion AP180 original host, raw Read, typed lock settings and GPIO erase; whole-image operations; hardware pending":"Exact Syntek STK6031/6032/6033 original workflows; complete raw Read, configuration and preservation; original package adapter required; hardware pending";}
            if(lt48_snas4_array_internal(r->definition))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==8||items[i].operation==10){items[i].availability=LT48_OPERATION_UNAVAILABLE;items[i].reason="Protection response/readback is unqualified; Secure and configuration are unavailable";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_ob16_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_ob16_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(((items[i].operation>=1&&items[i].operation<=6)||items[i].operation==8||items[i].operation==10)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="ob16_source_audited";items[i].reason="Exact On-Bright OB37R04A1/G1 and OB37R259A original workflows; complete raw Read, typed Configuration and Protect; standalone Erase unavailable; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(lt48_tms87_admitted_internal(entry.device_class,entry.device_type)){
        lt48_tms87_resources*r=NULL;if(!lt48_tms87_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=6){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="tms87two_source_audited";items[i].reason="TMS87 multiplexed EPROM: original raw reader, authored baseline-FPGA byte-pair programming, independent raw readback and source voltages; no electrical Erase; hardware pending";}
            lt48_tms87_close(r);return LT48_OK;
        }
    }
    if(lt48_tms370_admitted_internal(entry.device_class,entry.device_type)){
        lt48_tms370_resources*r=NULL;if(!lt48_tms370_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=6){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="tms370seven_source_audited";items[i].reason="TMS370 raw Read, sparse EPROM and optional EEPROM image; EEPROM blank is 00, EPROM blank is FF; EPROM electrical Erase unavailable; source adapter required; hardware pending";}
            lt48_tms370_close(r);return LT48_OK;
        }
    }
    if(lt48_st7_admitted_internal(entry.device_class,entry.device_type)){
        lt48_st7_resources*r=NULL;if(!lt48_st7_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="st7twelve_source_audited";items[i].reason="ST7 raw Read, sparse code/EEPROM image and original monitor operations; options preserved, explicit Erase clears read protection; irreversible write protection unavailable; original adapter required; hardware pending";}
            lt48_st7_close(r);return LT48_OK;
        }
    }
    if(lt48_r8c8_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_r8c8_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="r8c8_source_audited";items[i].reason="R8C source-audited raw Read and flash operations; erase before rewrite, single final OFS write; Protect disables further parallel access and cannot be cleared here; original adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(ite2_member(&entry)){
        lt48_s8253_resources*r=NULL;if(!lt48_ite2_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=7){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="ite2_source_audited";items[i].reason="ITE IT6350 audited source entry, unprotect, erase and byte workflows; full raw Read; no Secure or Configuration; original adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2035u)&&lt48_so30_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_so30_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(((items[i].operation>=1&&items[i].operation<=6)||items[i].operation==8||items[i].operation==10)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type==9?"fm8one_source_audited":(entry.device_type==0x11||entry.device_type==0x12)?"soB3_source_audited":"so30_source_audited";items[i].reason="Exact Sonix SN8P16/17/25/26 original workflows; complete raw Read, typed Configuration and Protect; standalone Erase unavailable; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_wt10_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_wt10_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="wt10_source_audited";items[i].reason="Exact Weltrend WT6702F/WT6703F/WT69P3 original workflows; complete raw Read, Erase; no Configuration/Secure; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_wt8_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_wt8_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="wt8_source_audited";items[i].reason="Exact Weltrend WT61P7/WT61P8/WT69P803 original workflows; complete raw Read, Erase; no Configuration/Secure; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_hy14_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_hy14_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=6 )||items[i].operation==8||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="hy14_source_audited";items[i].reason="Exact Hyundai HMS81020ET/32ET and HMS87C140xB original workflows; complete raw Read; no Erase; typed Configuration/Secure; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_mw6_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_mw6_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||items[i].operation==8||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="mw6_source_audited";items[i].reason="Exact Megawin MPC89L516X2/556X2 original workflows; raw Read and Erase; typed Configuration/Secure; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_my7_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_my7_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="my7_source_audited";items[i].reason="Exact Myson MTV212M64/MTV412M original workflows; raw Read and Erase; Configuration/Secure unavailable; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_my4_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_my4_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||(r->fields&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="my4_source_audited";items[i].reason="Exact Myson MTV312M64/MTV416M/MTV512M original workflows; raw Read and Erase; Protect configuration/Secure where defined; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_sc2_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sc2_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||(r->fields&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sc2_source_audited";items[i].reason="Exact SyncMos SM894051 original workflows; raw Read and Erase; Protect configuration/Secure where defined; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2048u&&lt48_sc4g_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sc4g_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||(r->fields&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sc4g_source_audited";items[i].reason="Exact SyncMos SM59R02G1 original workflows; raw Read and Erase; Protect configuration/Secure where defined; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_sc4a_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sc4a_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||(r->fields&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sc4a_source_audited";items[i].reason="Exact SyncMos SM59R08/16A2C/L original workflows; raw Read and Erase; Protect configuration/Secure where defined; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_tp4_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_tp4_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||(r->fields&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="tp4_source_audited";items[i].reason="Exact TOPRO TP2804 original workflows; raw Read and Erase; Write-only Protect/Secure; protection-state Read unavailable; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2048u&&lt48_tp6_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_tp6_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||(r->fields&&(items[i].operation==8||items[i].operation==10))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="tp6_source_audited";items[i].reason="Exact TOPRO TP2808 original workflows; raw Read and Erase; Write-only Protect/Secure; protection-state Read unavailable; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_mw8_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_mw8_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=7 )||items[i].operation==8||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="mw8_source_audited";items[i].reason="Exact Megawin MG86FE104/FL104 original workflows; raw Read and Erase; typed Configuration/Secure; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2038u&&lt48_gc8_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_gc8_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=6)||items[i].operation==8){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type==263?"gc520one_source_audited":"gc8_source_audited";items[i].reason="Exact GENCORE GC87C510A0 original workflows; complete raw Read and Secure; no Erase or Configuration; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2039u&&lt48_ab11_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_ab11_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="ab11_source_audited";items[i].reason="Exact ABOV MC81F42/43/44 original workflows; complete raw Read, Erase; typed Configuration/Secure; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2048u&&lt48_su16_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_su16_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=6)||items[i].operation==8||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="su16_source_audited";items[i].reason="Exact Sunplus SPMC65P original workflows; complete raw Read, typed Configuration/Secure; no Erase; original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_sm3_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sm3_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sm3_source_audited";items[i].reason="Exact SyncMos SM8951B original workflows; complete raw Read, write-only Protect (no protection-state Read); original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if((entry.device_class==2038u||entry.device_class==2048u)&&lt48_sm6_admitted_internal(entry.device_class,entry.device_type)){
        lt48_s8253_resources*r=NULL;if(!lt48_sm6_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sm6_source_audited";items[i].reason="Exact SyncMos SM89S16R1/SM89T16R1 original workflows; complete raw Read, write-only Protect (no protection-state Read); original package adapter required; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2018u&&entry.device_type>=0x112&&entry.device_type<=0x115&&!strcmp(entry.vendor,"Atmel")&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){
        lt48_s8253_resources*r=NULL;if(lt48_t89six_admitted_internal(entry.device_type)&&!lt48_t89six_open(installation,device_index,&r)){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="t89six_source_audited";items[i].reason="Atmel T89 full raw Read, boot/EEPROM image and preserved extra-row settings; original adapter; hardware pending";}
            lt48_s8253_close(r);
        }return LT48_OK;
    }
    if(lt48_ramtron21_member_internal(&entry)&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(!lt48_ramtron6_open(installation,device_index,&r)){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="ramtron21_source_audited";items[i].reason="Exact VRS51-compatible source alias; raw Read and nine operations; original adapter; protection action is write-only; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2038u&&!strcmp(entry.vendor,"RAMTRON")&&(entry.device_type==172u||entry.device_type==173u||entry.device_type==176u||entry.device_type==177u)&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_ramtron6_admitted_internal()&&!lt48_ramtron6_open(installation,device_index,&r)){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="ramtron6_source_audited";items[i].reason="VRS51 full raw Read; source Program erases first; boot-area Fuse preserved; protection action is write-only; original adapter; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2041u&&entry.device_type>=15u&&entry.device_type<=17u&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(!lt48_white3_open(installation,device_index,&r)){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=7){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="white3_source_audited";items[i].reason="White Electronic parallel EEPROM original Page workflows; full raw Read, verified full-image FF-fill Erase, source SDP write preference; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2041u&&(entry.device_type==18u||entry.device_type==19u)&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_eep2041_3_admitted_internal()&&!lt48_eep2041_3_open(installation,device_index,&r)){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=7){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="eep2041_3_source_audited";items[i].reason="Legacy parallel EEPROM,8192 bytes and source write-policy choice; raw Read; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2044u&&entry.device_type==230u&&!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_at28c64b1_admitted_internal()&&lt48_at28c64b1_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=7){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at28c64b1_source_audited";items[i].reason="Parallel EEPROM,8192+64 writable bytes, original A9 high-voltage selection and write-policy choice; raw Read; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2048u&&lt48_stm8s4_member_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_stm8s4_admitted_internal(entry.device_type)&&lt48_stm8s4_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="stm8s4_source_audited";items[i].reason="Original SWIM/option-pair procedures; raw Read; erased arrays00; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2048u&&lt48_cc254x5_member_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_cc254x5_admitted_internal(entry.device_type)&&lt48_cc254x5_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="cc254x5_source_audited";items[i].reason="Original CC254x debug streams, typed debug lock and raw Read; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2039u&&entry.device_type==5u){lt48_s8253_resources*r=NULL;if(lt48_n76one_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="n76one_source_audited";items[i].reason="Exact N76E003 original ICP workflows, raw Read, typed configuration and partition preservation; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2048u&&entry.device_type==205u){lt48_s8253_resources*r=NULL;if(lt48_sst58rc2_admitted_internal(entry.device_type)&&lt48_sst58rc2_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=1&&items[i].operation<=8)||items[i].operation==10){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sst58rc2_source_audited";items[i].reason="Original SST AddTab streams and typed configuration; raw Read; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2048u&&lt48_novatek25_member_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_novatek25_admitted_internal(entry.device_type)&&lt48_novatek25_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=7){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=r->novatek25_main_only?"novatek25_source_audited":"novatek_aux12_source_audited";items[i].reason="Original Novatek selected-region streams, raw Read and independent complete-image readback; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2048u&&(entry.device_type==0x1c8u||(entry.device_type>=0x1d1u&&entry.device_type<=0x1d5u))){lt48_s8253_resources*r=NULL;if(lt48_nuvoton8_admitted_internal(entry.device_type)&&lt48_nuvoton8_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="nuvoton8_source_audited";items[i].reason="Original Nuvoton mapped-memory workflows, raw Read and typed configuration; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2048u&&entry.device_type>=0x26du&&entry.device_type<=0x271u){lt48_s8253_resources*r=NULL;if(lt48_xmega7_admitted_internal(entry.device_type)&&lt48_xmega7_open(installation,device_index,&r)==LT48_OK){for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="xmega7_source_audited";items[i].reason="Original XMEGA PDI workflows, full raw Read, typed fuses and locks; hardware pending";}lt48_s8253_close(r);}return LT48_OK;}
    if(entry.device_class==2038u){lt48_s8253_resources*r=NULL;if(lt48_psoc29_open(installation,device_index,&r)==LT48_OK){
        if(lt48_psoc29_admitted_internal(entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=8){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=lt48_psoc39_member_internal(entry.device_type)?"psoc39_source_audited":"psoc29_source_audited";items[i].reason="Original PSoC ISSP workers and source host; raw Read; full-image Program/Overprogram erase first and clear security; Secure requires an explicit device-specific security record and returns source completion only; hardware pending";}
        lt48_s8253_close(r);return LT48_OK;}}
    if(entry.device_class==2038u||entry.device_class==2048u){
        lt48_s8253_resources*msp=NULL;if(lt48_msp43076_open(installation,device_index,&msp)==LT48_OK){
            if(lt48_msp43076_admitted_internal(entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type>=0x16d&&entry.device_type<=0x16f?"msp3_source_audited":msp->fields==1?"msp10_source_audited":"msp43076_source_audited";items[i].reason=msp->fields==1?"Exact old MSP430 workflows, full information/main raw Read and permanent JTAG fuse; hardware pending":"Exact MSP430 workflows, full raw Read, calibration preservation and permanent JTAG fuse; hardware pending";}
            lt48_s8253_close(msp);return LT48_OK;
        }
        lt48_s8253_resources*r=NULL;if(lt48_sstmx16_open(installation,device_index,&r)==LT48_OK){
            if(lt48_sstmx16_admitted_internal(entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sstmx16_source_audited";items[i].reason="Exact SST/Macronix original workflows, full raw Read across mapped regions, typed configuration and independent readback; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
        r=NULL;if(lt48_n79w79_21_open(installation,device_index,&r)==LT48_OK){
            if(lt48_n79w79_21_admitted_internal(entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="n79w79_21_source_audited";items[i].reason="Exact Nuvoton original workflows, full raw Read across mapped regions, typed configuration and independent readback; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
        r=NULL;if(lt48_nuvoton30_open(installation,device_index,&r)==LT48_OK){
            if(lt48_nuvoton30_admitted_internal(entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="nuvoton30_source_audited";items[i].reason="Exact Nuvoton original workflows, full raw Read across mapped regions, typed configuration and independent readback; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
        r=NULL;if(lt48_numicro81_open(installation,device_index,&r)==LT48_OK){
            if(lt48_numicro81_admitted_internal(entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type==414?"ilitek1_source_audited":"numicro81_source_audited";items[i].reason="Exact NuMicro original workflows, full raw Read across mapped regions, typed configuration and independent readback; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
        r=NULL;if(lt48_n79w79_open(installation,device_index,&r)==LT48_OK){
            if(lt48_n79w79_admitted_internal(entry.device_class,entry.device_type))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&items[i].operation!=9){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="n79w79_source_audited";items[i].reason="Exact N79/W79 source workflows, full raw Read and mapped memory regions, preserved options, configuration and independent readback; hardware pending";}
            lt48_s8253_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2048u){
        {
        lt48_lp51_basic_resources *lp=NULL;
        if(lt48_lp51_basic_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_lp51_basic_source_audited";
                items[i].reason="Exact AT89LP51/52 source resources; complete memory operations, code/data/user signature and restored temporary fuses, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_lp51_basic_close(lp);return LT48_OK;
        }
        }
        {
        lt48_s2051_resources *lp=NULL;
        if(lt48_s2051_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_s2051_source_audited";
                items[i].reason="Exact AT89S2051/S4051 source resources; complete memory operations, code/user signature and restored temporary fuses, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_s2051_close(lp);return LT48_OK;
        }
        }
        {
        lt48_s8253_resources *lp=NULL;
        if((entry.device_type<0x9a||entry.device_type>0xb1||lt48_w78bank147_admitted_internal(entry.device_type)) && (entry.device_type<0xce||entry.device_type>0xd3||lt48_w78d24_admitted_internal(entry.device_type)) && (entry.device_type<0xb2 || entry.device_type>0xcc || (entry.device_type<0xc2?lt48_sst_admitted_internal(entry.device_type):lt48_p89v_admitted_internal(entry.device_type))) && lt48_s8253_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=lp->kind==4?"w78bank147_source_audited":lp->kind==3?"w78d24_source_audited":lp->kind==2?"sst89rd_source_audited":lp->kind?"p89v51_source_audited":"at89_s8253_source_audited";
                items[i].reason=lp->kind==4?"Exact banked Winbond/Nuvoton workflows; raw Read, erase-once full-image programming, preserved non-lock options and source configuration; hardware pending":lp->kind==3?"Exact W78D workflows; two-region raw Read, full-image erase-before-program, preserved non-lock settings, six-field configuration; hardware pending":lp->kind==2?"Exact SST RD/RD2 source workflows; full two-region raw Read, erase, program, verification, security and configuration; hardware pending":lp->kind?"Exact P89V/LV51 source workflows, raw code/ISP block Read, erase, program, verification and configuration; hardware pending":"Exact AT89S8253 source resources; complete memory operations, code/EEPROM/user signature and restored temporary fuses, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_s8253_close(lp);return LT48_OK;
        }
        }
        {
        lt48_re2_resources *lp=NULL;
        if(lt48_re2_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_re2_source_audited";
                items[i].reason="Exact AT89C51RE2 source resources; 128-KiB two-bank code operations, guarded reads and FCB restoration after erase, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_re2_close(lp);return LT48_OK;
        }
        }
        {
        lt48_lp2052_resources *lp=NULL;
        if(lt48_lp2052_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_lp2052_source_audited";
                items[i].reason="Exact AT89LP2052/4052 source resources; complete memory operations, code/user signature and restored temporary fuses, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_lp2052_close(lp);return LT48_OK;
        }
        }
        {
        lt48_usb51_resources *lp=NULL;
        if(lt48_usb51_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_usb51_source_audited";
                items[i].reason="Exact AT89C5130/C5131 source resources; complete memory operations, preserved boot configuration and FF padding, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_usb51_close(lp);return LT48_OK;
        }
        }
        {
        lt48_ed2_resources *lp=NULL;
        if(lt48_ed2_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_ed2_source_audited";
                items[i].reason="Exact AT89C51ED2/ID2/CC03 source resources; complete memory operations, preserved boot configuration, verified settings before locks, independent readback; hardware qualification pending";
            }
            lt48_ed2_close(lp);return LT48_OK;
        }
        }
        {
        lt48_at89_lp51_resources *lp=NULL;
        if(lt48_at89_lp51_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_lp51_source_audited";
                items[i].reason="Exact AT89LP51ED2/ID2/RD2 source resources; code/EEPROM/user signature, preserved calibration page and fuses, explicit lock/configuration writes and independent readback; hardware qualification pending";
            }
            lt48_at89_lp51_close(lp);return LT48_OK;
        }
        }
        lt48_at89_lp_large_resources *lp=NULL;
        if(lt48_at89_lp_large_open(installation,device_index,&lp)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT){
                items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="at89_lp_large_source_audited";
                items[i].reason="Exact AT89LP428/828/3240/6440 source resources; code/data/user signature, preserved analog page and fuses, explicit lock/configuration writes and independent readback; hardware qualification pending";
            }
            lt48_at89_lp_large_close(lp);return LT48_OK;
        }
        lt48_pic24_resources *pic24=NULL;
        if(lt48_pic24_open(installation,device_index,&pic24)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if((items[i].operation>=LT48_OPERATION_READ && items[i].operation<=LT48_OPERATION_SECURE)||items[i].operation==LT48_OPERATION_CONFIGURATION){
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                if(lt48_pic24_capacity(pic24)==0x1f0001au){
                    items[i].support_level="dspic30_source_audited";
                    items[i].reason="Exact source-bound dsPIC30 C workflow;code/optional EEPROM/five-register configuration,source erase effects,independent readback and delayed protection;hardware qualification pending";
                }else if(lt48_pic24_capacity(pic24)==0x1f00020u){
                    items[i].support_level="pic24_klkm_source_audited";
                    items[i].reason="Exact source-bound PIC24 KL/KM C workflow;code/EEPROM/seven-register configuration,whole erase,independent readback and delayed protection;hardware qualification pending";
                }else if(lt48_pic24_capacity(pic24)==0x1f00024u){
                    items[i].support_level="pic24_ka_source_audited";
                    items[i].reason="Exact source-bound PIC24 KA C workflow;code/EEPROM/configuration,whole erase,independent readback and delayed protection;hardware qualification pending";
                }else{
                    items[i].support_level="pic24_fj_source_audited";
                    items[i].reason="Exact source-bound PIC24FJ C workflow;whole-image operations,source voltage verification before protection,configuration readback and code-preserving configuration writes;hardware qualification pending";
                }
            }
            lt48_pic24_close(pic24);return LT48_OK;
        }
        lt48_pic18_resources *r=NULL;
        if(lt48_pic18_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<8;i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="pic18_source_audited";items[i].reason="Digest-bound PIC code/data/configuration workflow; full-image programming, source voltage margins, delayed protection and independent readback; see per-device hardware scope";}
            if(entry.device_type!=0x20b&&entry.device_type!=0x20c)for(size_t i=0;i<8;i++) {
                items[i].support_level="pic18_static_cohort_source_audited";
                items[i].reason="Exact source-bound PIC18 cohort; complete C workflows, manufacturer-bound code/EEPROM/configuration/protection/erase replay, delayed protection and failure cleanup; hardware qualification pending";
            }
            if(lt48_pic18_is_j_series(r))for(size_t i=0;i<8;i++) {
                items[i].support_level="pic18_j_source_audited";
                items[i].reason="Exact PIC18 J alias/source/carrier;64-byte one-write Flash,embedded configuration,pre-reset verification,explicit erase/rewrite consent and stable preservation backup;hardware qualification pending";
            }
            lt48_pic18_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2000u){
        lt48_compact_i2c_resources *r=NULL;
        if(lt48_compact_i2c_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<(lt48_compact_i2c_has_protection(r)?8u:7u);i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=entry.device_type==0xe5?"x4c105_source_audited":entry.device_type==0xbe?"x84041_source_audited":entry.device_type==0xfffc?"x24641_native_procedure":"compact_i2c_source_audited";items[i].reason="Exact selected compact I2C procedure; nonblank rewrites, preserved partial chunks, margin and fresh-power verification; clear writes FF";}
            if(lt48_compact_i2c_is_at17(r)){
                int aec1430=!strcmp(lt48_installation_source_profile(installation),"AEC 14.30");
                for(size_t i=0;i<7;i++){items[i].support_level=aec1430?"at17_compact_aec1430_audited":lt48_compact_i2c_is_at17(r)==4?"at17_applet_source_audited":lt48_compact_i2c_is_at17(r)>=2?"at17_large_source_audited":"at17_compact_source_audited";items[i].reason="Original compact AT17 memory procedures with preserved configuration and independent readback; audit only";}
                items[LT48_OPERATION_CONFIGURATION-1].availability=LT48_OPERATION_EXECUTABLE;
                items[LT48_OPERATION_CONFIGURATION-1].support_level=aec1430?"at17_compact_aec1430_audited":lt48_compact_i2c_is_at17(r)==4?"at17_applet_source_audited":lt48_compact_i2c_is_at17(r)>=2?"at17_large_source_audited":"at17_compact_source_audited";
                items[LT48_OPERATION_CONFIGURATION-1].reason="Explicit RESET/OE polarity and source-supported oscillator setting; preserved during memory writes, independently checked with unchanged array; audit only";
            }
            if(lt48_compact_i2c_spd_protection_bits(r)) {
                for(size_t i=0;i<8;i++)items[i].reason="SPD source waves; preserve reversible protection across writes and cancellation, bank-aware full-array verification; hardware qualification pending";
                items[LT48_OPERATION_CONFIGURATION-1].availability=LT48_OPERATION_EXECUTABLE;
                items[LT48_OPERATION_CONFIGURATION-1].support_level="compact_i2c_source_audited";
                items[LT48_OPERATION_CONFIGURATION-1].reason="Explicit protection mask with readback; STTS permanent lower-half lock cannot be cleared";
                items[LT48_OPERATION_SECURE-1].reason="Explicit SPD protection mask; four reversible blocks or STTS lower-half permanent/reversible settings";
            }
            if(lt48_compact_i2c_has_endurance_config(r)) {
                for(size_t i=0;i<8;i++)items[i].reason="24C65 source commands with full-byte configuration capture; preserve all three fields, verify array and settings; hardware qualification pending";
                items[LT48_OPERATION_SECURE-1].reason="Explicit one-time security start/count; existing high-endurance block takes precedence; configuration and array readback";
                items[LT48_OPERATION_CONFIGURATION-1].availability=LT48_OPERATION_EXECUTABLE;
                items[LT48_OPERATION_CONFIGURATION-1].support_level="compact_i2c_source_audited";
                items[LT48_OPERATION_CONFIGURATION-1].reason="Explicit high-endurance block selector; rejected if already locked to a different setting; original wave and independent readback";
            }
            if(lt48_compact_i2c_has_half_protection(r)) {
                items[6].reason="Fill FF with full readback; rejected before writes if it would change protected lower-half bytes; does not unlock";
                items[7].reason="Original source Secure locks 0000..007f; no unlock in this procedure; ACK status and unchanged array independently verified";
            }
            lt48_compact_i2c_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2000u){
        lt48_microwire_resources *r=NULL;
        if(lt48_microwire_open(installation,device_index,&r)==LT48_OK){
            int nvram=lt48_microwire_is_nvram(r);
            for(size_t i=0;i<(nvram?6u:7u);i++){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=nvram?"serial_nvram_source_audited":"microwire_source_audited";items[i].reason=nvram?"Exact X24C44 or CAT24C44 source SRAM/RECALL/STORE procedure; native readback and fresh-power full-array persistence verification":"Exact Microwire 93C source closure; native serial write/read/ready poll and whole-device erase with post-verification";}
            if(nvram)items[6].reason="Clear by programming an all-FF buffer; STORE and persistence verification are included";
            lt48_microwire_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u&&!strcmp(entry.vendor,"SyncMos")){lt48_small_eprom_resources*r=NULL;if(!lt48_small_eprom_open(installation,device_index,&r)){if(lt48_small_eprom_is_sm2965four(r)){for(unsigned k=0;k<LT48_OPERATION_KIND_COUNT;k++)if((k<8||k==9)){items[k].availability=LT48_OPERATION_EXECUTABLE;items[k].support_level="sm2965four_source_audited";items[k].reason="Original SyncMos raw Read/code/erase/options/protection, preserved settings and independent readback; hardware pending";}lt48_small_eprom_close(r);return LT48_OK;}lt48_small_eprom_close(r);}}
    if(entry.device_class==2014u){lt48_hc705_bootstrap_resources *r=NULL;
        if(lt48_hc705_bootstrap_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_bootstrap_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_bootstrap_source_audited":"hc705_bootstrap_offline_owner";items[i].reason=qualified?"Original HC705 bootstrap workflows; sparse EPROM image and fresh raw readback":"Native HC705 bootstrap owner under original-workflow qualification; live operations disabled";}
            lt48_hc705_bootstrap_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u){lt48_hc705_jp_resources *r=NULL;
        if(lt48_hc705_jp_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_jp_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_jp_source_audited":"hc705_jp_offline_owner";items[i].reason=qualified?"Original HC705 J/P bootstrap workflows; sparse EPROM image and fresh raw readback":"Native HC705 J/P bootstrap owner under original-workflow qualification; live operations disabled";}
            lt48_hc705_jp_close(r);return LT48_OK;
        }
    }    if(entry.device_class==2014u){lt48_hc705_ca_resources *r=NULL;
        if(lt48_hc705_ca_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_ca_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_ca_source_audited":"hc705_ca_offline_owner";items[i].reason=qualified?"Original HC705 C8A/C9A bootstrap workflows; sparse EPROM image and fresh raw readback":"Native HC705 C8A/C9A bootstrap owner under original-workflow qualification; live operations disabled";}
            lt48_hc705_ca_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u){lt48_hc705_bd_resources *r=NULL;
        if(lt48_hc705_bd_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_bd_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_bd_source_audited":"hc705_bd_offline_owner";items[i].reason=qualified?"Original HC705 BD3/BD7/BD9 bootstrap workflows; sparse EPROM image and fresh raw readback":"Native HC705 BD3/BD7/BD9 bootstrap owner under original-workflow qualification; live operations disabled";}
            lt48_hc705_bd_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u){lt48_hc705_bx_resources *r=NULL;
        if(lt48_hc705_bx_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_bx_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_bx_source_audited":"hc705_bx_offline_owner";items[i].reason=qualified?"Original HC705 B16/B32/X32/B5 bootstrap workflows; sparse EPROM image and fresh raw readback":"Native HC705 B16/B32/X32/B5 bootstrap owner under original-workflow qualification; live operations disabled";}
            lt48_hc705_bx_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u){lt48_rs08_resources *r=NULL;
        if(lt48_rs08_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_rs08_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE||items[i].operation==LT48_OPERATION_ERASE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"rs08_source_audited":"rs08_offline_owner";items[i].reason=qualified?"Original RS08 BDM workflows; sparse Flash image and fresh raw readback":"Native RS08 BDM owner under original-workflow qualification; live operations disabled";}
            lt48_rs08_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u){lt48_s08_resources *r=NULL;
        if(lt48_s08_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_s08_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE||items[i].operation==LT48_OPERATION_ERASE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"s08_source_audited":"s08_offline_owner";items[i].reason=qualified?"Original S08 BDM workflows; sparse Flash image and fresh raw readback":"Native S08 BDM owner under original-workflow qualification; live operations disabled";}
            lt48_s08_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2014u){lt48_hc705_jjsrpl_resources *r=NULL;
        if(lt48_hc705_jjsrpl_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_jjsrpl_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_jjsrpl_source_audited":"hc705_jjsrpl_offline_owner";items[i].reason=qualified?"Original HC705 JJ7/JP7/SR3/PL4 bootstrap workflows; sparse EPROM image and fresh raw readback":"Native HC705 JJ7/JP7/SR3/PL4 bootstrap owner under original-workflow qualification; live operations disabled";}
            lt48_hc705_jjsrpl_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2012u){lt48_hc705_compact_resources *r=NULL;
        if(lt48_hc705_compact_open(installation,device_index,&r)==LT48_OK){
            int qualified=lt48_hc705_compact_qualified(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=qualified?LT48_OPERATION_EXECUTABLE:LT48_OPERATION_PLAN_ONLY;items[i].support_level=qualified?"hc705_compact_source_audited":"hc705_compact_offline_owner";items[i].reason=qualified?"Original compact HC705 procedures; preserved MOR and independent readback":"Native C owner under offline audit; factory-mode target effects and complete workflow qualification pending; live operations disabled";}
            lt48_hc705_compact_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2013u){lt48_nmos68705_resources *r=NULL;
        if(lt48_nmos68705_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=LT48_OPERATION_PLAN_ONLY;items[i].support_level="nmos68705_offline_owner";items[i].reason="Native C owner implemented for offline audit; NUM electrical/variant qualification pending. Series 1 kohm mode-pin adapter required; live operations disabled";}
            lt48_nmos68705_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2013u){lt48_hc711_resources *r=NULL;
        if(lt48_hc711_open(installation,device_index,&r)==LT48_OK){
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation==LT48_OPERATION_READ||items[i].operation==LT48_OPERATION_BLANK_CHECK||items[i].operation==LT48_OPERATION_PROGRAM||items[i].operation==LT48_OPERATION_VERIFY||items[i].operation==LT48_OPERATION_COMPARE){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="hc711_eprom_source_audited";items[i].reason="HC711 original bootstrap/UART/pin setup; timed authored EPROM kernel, preserved nonvolatile CONFIG and independent margin readback. Original adapter required; hardware qualification pending";}
            lt48_hc711_close(r);return LT48_OK;
        }
    }
    if (entry.device_class == 2013u) {
        lt48_hc11_resources *r=NULL;
        if(lt48_hc11_open(installation,device_index,&r)==LT48_OK) {
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++) {
                lt48_operation_kind op=items[i].operation;
                if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_CONFIGURATION)continue;
                if(lt48_hc11_is_extension(r)&&op==LT48_OPERATION_CONFIGURATION)continue;
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level=lt48_hc11_is_extension(r)?"hc11_eeprom_extensions_source_audited":"hc11_ael_source_audited";
                items[i].reason="HC11 original bootstrap, EEPROM and CONFIG procedures with independent readback; 512-byte EEPROM image, CONFIG preserved. Secured-mask bootstrap inherently erases. Clearing NOSEC deferred; hardware pending";
            }
            if(lt48_hc11_is_extension(r))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].availability==LT48_OPERATION_EXECUTABLE)items[i].reason="HC11 E2/F1/K original EEPROM worker, CONFIG writes excluded to preserve nonvolatile settings; whole EEPROM image, independent readback. CONFIG editing deferred; secured-mask bootstrap can erase before access. Original package adapter required; hardware pending";
            lt48_hc11_close(r);return LT48_OK;
        }
    }
    int st62_owner=0,el5_owner=0;
    if(entry.device_class==2013u && !strcmp(entry.vendor,"ST")) {
        lt48_small_eprom_resources *sr=NULL;
        if(lt48_small_eprom_open(installation,device_index,&sr)==LT48_OK) {
            st62_owner=lt48_small_eprom_is_st62(sr);lt48_small_eprom_close(sr);
        }
    }
    if(entry.device_class==2013u&&!strcmp(entry.vendor,"ELAN")){lt48_small_eprom_resources*sr=NULL;if(!lt48_small_eprom_open(installation,device_index,&sr)){el5_owner=lt48_small_eprom_is_elan5(sr)!=0;lt48_small_eprom_close(sr);}}
    if ((entry.device_class == 2013u && !st62_owner && !el5_owner && entry.device_type!=0xc5 && (strcmp(entry.vendor,"Zilog")||(entry.device_type>=0xa8&&entry.device_type<=0xab))) || entry.device_class == 2018u) {
        lt48_mcu51_resources *mcu = NULL;
        if (lt48_mcu51_open(installation, device_index, &mcu) == LT48_OK) {
            if(lt48_mcu51_cl38(mcu)){
                if(lt48_mcu51_cl38_admitted(mcu))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&(lt48_mcu51_cl38_mask(mcu)&(1u<<(items[i].operation-1)))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="class2018_38_source_audited";items[i].reason="Exact class2018 source workflows and raw Read; preserved options and independent readback; hardware pending";}
                lt48_mcu51_close(mcu);return LT48_OK;
            }
            if(lt48_mcu51_lm20_kind(mcu)){
                if(lt48_mcu51_lm20_admitted(mcu))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation>=1&&items[i].operation<=10&&(lt48_mcu51_lm20_mask(mcu)&(1u<<(items[i].operation-1)))){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=lt48_mcu51_lm20_kind(mcu)==8?"hms99one_source_audited":"legacy_mcu20_source_audited";items[i].reason="Exact legacy MCU original workflows and raw Read; selected package adapter required; hardware pending";}
                lt48_mcu51_close(mcu);return LT48_OK;
            }
            unsigned l14=lt48_mcu51_legacy14_kind(mcu);
            if(l14){
                unsigned char*blank=malloc(lt48_mcu51_capacity(mcu));if(!blank){lt48_mcu51_close(mcu);return LT48_NO_MEMORY;}memset(blank,255,lt48_mcu51_capacity(mcu));
                if(lt48_mcu51_legacy14_admitted(mcu))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(lt48_mcu51_validate(mcu,items[i].operation,0,lt48_mcu51_capacity(mcu),(items[i].operation>=3&&items[i].operation<=6)?blank:NULL,(items[i].operation>=3&&items[i].operation<=6)?lt48_mcu51_capacity(mcu):0)==LT48_OK){
                    items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level=l14==10?"aduc812one_source_audited":(l14==8||l14==9)?"otp13five_source_audited":"legacy14_source_audited";items[i].reason="Exact legacy MCU source workflows, raw Read, profile-specific geometry/configuration, independent readback; original adapter required; hardware pending";}
                free(blank);lt48_mcu51_close(mcu);return LT48_OK;
            }
            int f41=lt48_mcu51_is_flash51_41(mcu),upi=lt48_mcu51_is_upi41ah(mcu),mcs48=lt48_mcu51_security_levels(mcu)==0,at89=lt48_mcu51_is_at89(mcu);
            int erasable=lt48_mcu51_validate(mcu,LT48_OPERATION_ERASE,0,lt48_mcu51_capacity(mcu),NULL,0)==LT48_OK;
            unsigned philips=lt48_mcu51_philips_options(mcu);
            int temic=entry.device_class==2018u && !strcmp(entry.vendor,"TEMIC") && entry.device_type>=0x95 && entry.device_type<=0x9c;
            for (size_t i = 0; i < (at89==2 || upi || philips>=2 || (f41>=16&&f41<=19)?10u:8u); ++i) {
                if(i==LT48_OPERATION_MEMORY_PROTECT-1u)continue;
                if ((!erasable && i == LT48_OPERATION_ERASE - 1u) || (mcs48 && i == LT48_OPERATION_SECURE - 1u)) continue;
                items[i].availability = LT48_OPERATION_EXECUTABLE;
                lt48_device_spec legacy_spec;int legacy9=lt48_installation_resolve_device(installation,device_index,&legacy_spec)==LT48_OK && legacy51_9_binding(&legacy_spec)!=NULL;
                int my2=lt48_installation_resolve_device(installation,device_index,&legacy_spec)==LT48_OK&&myson2_binding(&legacy_spec)!=NULL;
                int my44=myson44_binding(&legacy_spec)!=NULL;
                int myM=mysonM2_binding(&legacy_spec)!=NULL;
                items[i].support_level = myM?"mysonM2_source_audited":my44?"myson44_source_audited":my2?"myson2_source_audited":lt48_mcu51_is_hy24_internal(mcu)?"hyundai24_source_audited":legacy9?"legacy51_9_source_audited":lt48_mcu51_is_mcu70(mcu)?"mcu70_source_audited":lt48_mcu51_is_flash51_57(mcu)?"flash51_57_source_audited":lt48_mcu51_is_flash51_41(mcu)?"flash51_41_source_audited":lt48_mcu51_is_otp20(mcu)?"otp20_source_audited":philips?"philips39_source_audited":temic?"temic26_source_audited":upi?"upi41ah_datasheet_audited":at89==2?"at89_legacy_s_source_audited":at89?"at89_legacy_lv_source_audited":mcs48?"mcs48_source_audited":"mcu51_source_audited";
                items[i].reason = myM?"MYSON MTP: original byte/banked raw Read, Program/Over, Verify, Compare, Blank and electrical Erase; no source Secure/ID query; original package adapter required; hardware pending":(my2||my44)?"MYSON OTP: original raw Read, programming and one protection procedure; no ID check or direct protection status readback; hardware pending":lt48_mcu51_is_hy24_internal(mcu)?"Exact Hyundai/Versachips/Weltrend source workflows; raw code Read, original programming/encryption/protection and cleanup; package adapter required; hardware pending":legacy9?"Exact Atmel/AMIC legacy OTP procedures, raw code Read and cumulative three-level protection; source package adapter required; hardware pending":f41?"Exact legacy flash8051 code/erase/protection callbacks, source voltages and canonical FF file holes; banked SST requires whole images; hardware pending":philips?"Exact Philips/Tekmos OTP multipulse, code/encryption/security and applicable clock settings; source package adapter required, hardware pending":temic?"TEMIC OTP 8051: original code/encryption workflows, explicit cumulative lock pulses; source package adapter required, hardware pending":upi?"Exact M4f FPGA and pin maps with authored Intel AH programming, signature and security workflows; hardware qualification pending":at89==2?"Exact AT89S/LS parallel code/EEPROM, erase, lock and SPI fuse workflows; hardware qualification pending":at89?"Exact AT89LV source closure; full-image autoerase programming, electrical erase and three protection levels":mcs48?"Exact MCS-48 source procedure; zero-erased array, monotonic 0-to-1 programming and independent margin readback":"Exact 8751/87C51 source closure with native C code, encryption-array and explicit protection workflows";
            }
            if(erasable && !at89 && !f41)for(size_t i=0;i<8;i++)items[i].reason="Exact class2018 flash8051 source callbacks, selected voltage margins, electrical erase and protection; hardware qualification pending";
            if(!erasable)items[6].reason = "EPROM/OTP MCU: no electrical erase; windowed parts require external UV erasure";
        }
        lt48_mcu51_close(mcu);
        return LT48_OK;
    }
    if (entry.device_class == 2034u && !(entry.device_type>=0x2a && entry.device_type<=0x2f)) {
        lt48_gdac_serial_resources *r=NULL;
        if(lt48_gdac_serial_open(installation,device_index,&r)==LT48_OK) {
            int otp=lt48_gdac_serial_is_otp(r);
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++) {
                lt48_operation_kind op=items[i].operation;
                if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)continue;
                if(otp&&op==LT48_OPERATION_ERASE)continue;
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level="gdac_serial_source_audited";
                items[i].reason=otp?"Serial OTP EPROM: original high-voltage applet, blank-before-program, voltage-margin verification and independent readback; no electrical erase; hardware testing pending":"Exact GDAC source applets, FPGA, pin map and sector unlock geometry; preserving writes/erase, original voltage margins and independent readback; hardware testing pending";
            }
            if(otp)items[6].reason="OTP EPROM: no electrical erase; programming FF cannot clear programmed bits";
            lt48_gdac_serial_close(r);return LT48_OK;
        }
    }
    if(entry.device_class==2009u&&entry.device_type>=0x64u&&entry.device_type<=0x68u){
        lt48_gaaf_serial_resources*r=NULL;if(!lt48_gaaf_serial_open(installation,device_index,&r)){
            if(lt48_gaaf_serial_qualified(r))for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++){unsigned op=items[i].operation;if(op>=1&&op<=7&&(op!=7||entry.device_type==0x66||entry.device_type==0x67)){items[i].availability=LT48_OPERATION_EXECUTABLE;items[i].support_level="sep7_source_audited";items[i].reason="Original serial EPROM workers and source pinmap; full raw Read; whole-image blank-before-Program/1-to0 Overprogram, independent raw margin verification; electrical Erase only for Winbond; hardware pending";}}
            lt48_gaaf_serial_close(r);return LT48_OK;
        }
    }
    if (entry.device_class == 2009u || entry.device_class == 2002u || entry.device_class == 2007u || entry.device_class == 2003u || entry.device_class == 2008u || entry.device_class == 2004u || entry.device_class == 2005u || entry.device_class == 2029u) {
        lt48_gaaf_serial_resources *r=NULL;
        if(lt48_gaaf_serial_open(installation,device_index,&r)==LT48_OK) {
            if(!lt48_gaaf_serial_qualified(r)||(!mx27pageone_member(&entry)&&!lt48_gaaf_serial_is_word14_internal(r)&&lt48_gaaf_serial_validate(r,LT48_OPERATION_ERASE,0,lt48_gaaf_serial_capacity(r),NULL,0,LT48_VERIFY_MARGIN_ALL,0)!=LT48_OK)){lt48_gaaf_serial_close(r);r=NULL;}
        }
        if(r) {
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++) {
                lt48_operation_kind op=items[i].operation;
                if(mx27pageone_member(&entry)&&op>LT48_OPERATION_COMPARE)continue;
                if(op==LT48_OPERATION_SECURE||op==LT48_OPERATION_CONFIGURATION){if(lt48_gaaf_serial_configuration_bits(r)<8&&!(sm2964two_member(&entry)&&op==LT48_OPERATION_SECURE))continue;}
                else if(!((entry.device_class==2002u||entry.device_class==2007u||entry.device_class==2003u||entry.device_class==2008u||entry.device_class==2004u||entry.device_class==2005u||entry.device_class==2029u||st2009two_member(&entry))&&op==LT48_OPERATION_OVER_PROGRAM)&&op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)continue;
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                const struct lt48_batch_admission *batch=lt48_batch_find(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length);
                items[i].support_level=(entry.device_class==2008&&entry.device_type==0x3f)?"mx26aone_source_audited":(entry.device_class==2004&&entry.device_type==3&&!strcmp(entry.vendor,"SST"))?"sst38one_source_audited":w27e4096two_member(&entry)?"w27e4096two_source_audited":mxwordtwo_member(&entry)?"mxwordtwo_source_audited":mx27pageone_member(&entry)?"mx27pageone_source_audited":(entry.device_class==2004&&!strcmp(entry.vendor,"Micron")&&(entry.device_type==0x87||entry.device_type==0x88))?"micronc3two_source_audited":(entry.device_class==2003&&!strcmp(entry.vendor,"SHARP")&&(entry.device_type==0x168||entry.device_type==0x169))?"sharpwordpulse3_source_audited":(entry.device_class==2004&&entry.device_type>=0x7e&&entry.device_type<=0x81&&!strcmp(entry.vendor,"SHARP"))?"sharpminato4_source_audited":w29wordtwo_member(&entry)?"w29wordtwo_source_audited":pss2005one_member(&entry)?"pss2005one_source_audited":x88one_member(&entry)?"x88one_source_audited":entry.device_class==2009&&entry.device_type==0x8c&&!strcmp(entry.vendor,"ISSI")?"issi2_source_audited":entry.device_class==2009&&entry.device_type==0x6c&&!strcmp(entry.vendor,"Great Tek")?"gt040one_source_audited":entry.device_class==2009&&!strcmp(entry.vendor,"PMC (Chingis)")&&(entry.device_type==0x8a||entry.device_type==0x8c||entry.device_type==0x8d)?"pmc6_source_audited":sm2964two_member(&entry)?"sm2964two_source_audited":efst1_member(&entry)?"efst1_source_audited":entry.device_class==2003u&&entry.device_type>=0x34&&entry.device_type<=0x37&&!strcmp(entry.vendor,"UTRON")?"utron4_source_audited":(entry.device_class==2003&&!strcmp(entry.vendor,"SHARP")&&(entry.device_type==0xc9||entry.device_type==0xcf||entry.device_type==0xd0||entry.device_type==0xd4||entry.device_type==0xd5||entry.device_type==0xd6||entry.device_type==0xd9))?"sharpwordseven_source_audited":(entry.device_class==2003&&entry.device_type>=0x138&&entry.device_type<=0x13a)?"sst32four_source_audited":(entry.device_class==2007&&((entry.device_type>=0xb8&&entry.device_type<=0xba&&!strcmp(entry.vendor,"Greenliant"))||(entry.device_type==0xb6&&!strcmp(entry.vendor,"PMC (Chingis)"))))?"sst37aliases_source_audited":(entry.device_class==2004&&entry.device_type==0x50)?"sharp1306_source_audited":(entry.device_class==2007&&entry.device_type>=0xb7&&entry.device_type<=0xba)?"sst37four_source_audited":(entry.device_class==2008&&(entry.device_type==0x17||entry.device_type==0x18||(entry.device_type>=0x71&&entry.device_type<=0x78)))?"sharp10_source_audited":entry.device_class==2029&&entry.device_type>=0x10&&entry.device_type<=0x19?"sharp11_source_audited":((entry.device_class==2007&&entry.device_type==0x9c)||(entry.device_class==2008&&(entry.device_type==0x39||entry.device_type==0x3a)))?"nor3_source_audited":entry.device_class==2009&&entry.device_type>=0x5b&&entry.device_type<=0x62&&entry.device_type!=0x5f?"lnand10_source_audited":lt48_gaaf_serial_is_word14_internal(r)?"word14_source_audited":lt48_gaaf_serial_is_combined36(r)?"combined36_source_audited":entry.device_class==2003u&&(entry.device_type==0x149u||entry.device_type==0x14au)?"samsung_word2_source_audited":ams_nor(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"ams_nor_source_audited":sharp_s3(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"sharp_s3_source_audited":fujitsu_002(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"fujitsu_002_authored_id_audited":atmel_lv2048b(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"atmel_lv2048b_authored_nc_audited":w49_t16(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"w49_t16_authored_id_audited":w49_dummy(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"w49_dummy_source_audited":(entry.device_class==2007&&entry.device_type==0x6c&&!strcmp(entry.vendor,"Macronix"))?"mx26bone_source_audited":mx26b(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"mx26b_authored_erase_check_audited":m28f800(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"m28f800_authored_status_audited":m29f800(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"m29f800_authored_status_audited":batch?batch->level:nexflash_010(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"nexflash_010_source_audited":macronix_lv040(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"macronix_lv040_source_audited":actrans4(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"actrans4_source_audited":macronix_f1601(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"macronix_f1601_source_audited":macronix_l3211(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"macronix_l3211_source_audited":macronix_l8000(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"macronix_l8000_source_audited":macronix_l8100(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"macronix_l8100_source_audited":macronix_psop3(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"macronix_psop3_source_audited":mx28f1000_auto(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"mx28f1000_authored_auto_audited":st28(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"legacy_st28_preconditioned_audited":sst1(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"legacy_sst1_source_audited":mx28p2(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"legacy_mx28p2_source_audited":alliance2(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"legacy_alliance2_source_audited":macronix_bootblock4(entry.device_class,entry.device_type,entry.manufacturer_id,entry.device_id,entry.vendor,entry.device_name,entry.device_name_length)?"legacy_macronix_bootblock4_source_audited":entry.device_class==2005u?"legacy_macronix6_source_audited":entry.device_class==2004u?"legacy_intel_f3_source_audited":entry.device_class==2008u?"legacy_route45_source_audited":entry.device_class==2003u?((entry.device_type==0x16u||entry.device_type==0xa0u||entry.device_type==0xa1u||entry.device_type==0x165u)?"legacy_mx_st_word7_source_audited":(entry.device_type==0xa3u||entry.device_type==0xa4u)?"legacy_winbond_word2_source_audited":(entry.device_type>=0x4du && entry.device_type<=0x54u)?"legacy_hynix8_source_audited":(entry.device_type>=0xdau && entry.device_type<=0xdfu)?"legacy_atmel_small7_source_audited":(entry.device_type>=0xe0u && entry.device_type<=0xebu)?"legacy_atmel_classic12_source_audited":(entry.device_type>=0xf5u && entry.device_type<=0x107u && entry.device_type!=0xfeu)?"legacy_atmel20_authored_timeout_audited":"legacy_word25_source_audited"):entry.device_class==2007u?((entry.device_type>=0x48u && entry.device_type<=0x4du)?"legacy_intel8_composed_erase_audited":(entry.device_type>=0xc2u && entry.device_type<=0xc4u)?"legacy_byte49_composed_erase_audited":(entry.device_type==0x85u||entry.device_type==0x86u||entry.device_type==0xc0u)?(entry.manufacturer_id==0x89u?"legacy_byte49_shared_erase_audited":"legacy_byte49_source_audited"):(entry.device_type==0x11u||entry.device_type==0x13u||entry.device_type==0x1au||entry.device_type==0x1bu||entry.device_type==0x1cu||entry.device_type==0x1du||entry.device_type==0xa2u||entry.device_type==0xa3u||entry.device_type==0xbu||entry.device_type==0xcu||entry.device_type==0xdu||entry.device_type==0xeu)?"legacy_nor28_source_audited":"legacy_nor53_source_audited"):entry.device_class==2002u?"legacy28f_source_audited":"gaaf_serial_source_audited";
                items[i].reason=entry.device_class==2005u?"Source-bound x16 page NOR; partial-page writes require whole affected pages blank; independent readback and protection-state restoration; hardware pending":(entry.device_class==2007u||entry.device_class==2003u||entry.device_class==2008u||entry.device_class==2004u||entry.device_class==2005u||entry.device_class==2029u)?"Original legacy NOR procedures, per-operation RAM applets, source unprotect prerequisites, voltage margins and independent readback; standalone protection deferred; hardware testing pending":entry.device_class==2002u?"Original class-2002 applets and pin map; blank-before-program, zero-fill then electrical erase, voltage margins and independent readback; hardware testing pending":op==LT48_OPERATION_CONFIGURATION?"Explicit source nonvolatile configuration bits using original mode8 INIT/PROTECT/RESET, independent readback; hardware testing pending":op==LT48_OPERATION_SECURE?"Protect whole array while preserving pin function, clock edge and direction; configuration can unlock; hardware testing pending":"Exact GAAF source applets, FPGA and pin map; preserving writes/erase, voltage margins and independent readback; Protection restored after mutation or failure; hardware testing pending";
                if(st2009two_member(&entry))items[i].support_level="st2009two_source_audited";
                if(st2009two_member(&entry))items[i].reason="ST M39 flash/EEPROM and raw OTP Read; seven array operations, full-image mutations, read-only OTP, unchanged sector locks/SDP, independent margin and fresh-power readback; hardware pending";
                if(mxwordtwo_member(&entry))items[i].reason="Macronix x16 page flash; seven memory operations and raw Read, protection unchanged, independent readback; hardware pending";
                if(mx27pageone_member(&entry))items[i].reason="MX27C1610 page OTP,128-byte pages; six memory operations, affected pages blank before writes, independent readback; no electrical Erase; hardware pending";
                if(w29wordtwo_member(&entry))items[i].reason="W29C101/102 word-page flash; full raw Read,256-byte pages, worker ACK checked, powered erase dwell and independent readback; hardware pending";
                if(pss2005one_member(&entry))items[i].reason="PSS128-byte page NOR; full raw Read, blank affected pages before writing, all12sector protection bits checked before mutation; protected writes/erase refused without unlocking; independent readback; hardware pending";
                if(x88one_member(&entry))items[i].reason="X88C64 multiplexed32-byte EEPROM; full raw Read, preserving partial overwrites, FF clear by programming, original block-unprotect before writes, independent readback; hardware pending";
                if(sm2964two_member(&entry))items[i].reason="Exact SyncMos SM2964 source workflows; full raw Read and independently verified writes/erase; Secure applies the original pulse without independent lock-status readback. Original adapter required; hardware pending";
                if(lt48_gaaf_serial_is_word14_internal(r)&&lt48_gaaf_serial_validate(r,op,0,lt48_gaaf_serial_capacity(r),NULL,0,LT48_VERIFY_MARGIN_ALL,0)==LT48_UNSUPPORTED){items[i].availability=LT48_OPERATION_UNAVAILABLE;items[i].reason="Mask ROM: programming, erase and protection changes unavailable";}
                if(entry.device_class==2009&&entry.device_type>=0x5b&&entry.device_type<=0x62&&entry.device_type!=0x5f)items[i].reason="Original raw NAND physical image; whole-image Program erases first; independent raw readback/margins; no ECC or bad-block remapping; Erase clears the source-defined image including marker bytes; hardware pending";
                if(!strcmp(items[i].support_level,"sharpwordseven_source_audited"))items[i].reason="Exact source word NOR procedures and full raw Read; every source sector lock status is checked without a 32-bit truncation. Locked mutations are refused; standalone protection editing is unavailable. Flash image excludes companion SRAM. Original package/Minato adapter required; hardware pending";
                if(lt48_gaaf_serial_is_combined36(r))items[i].reason="Original class2004 host, main flash plus command88 window, exact workers/FPGA/adapters; protected mutations refused; full-image writes, original margins, independent raw readback and protection preservation; hardware pending";
            }
            if(entry.device_class==2003u && entry.device_type>=0xf5u && entry.device_type<=0x107u && entry.device_type!=0xfeu)
                for(size_t i=0;i<7;i++)items[i].reason="Source-audited word NOR with exact NewDeviceLib timeout extension; successful program path preserved; independent verification and cleanup; hardware pending";
            lt48_gaaf_serial_close(r);return LT48_OK;
        }
    }
    if (entry.device_class == 2014u) {
        lt48_hc908_resources *r=NULL;
        if(lt48_hc908_open(installation,device_index,&r)==LT48_OK) {
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++) {
                lt48_operation_kind op=items[i].operation;
                if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)continue;
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level=lt48_hc908_is_fqtqy(r)?"hc908_fqtqy_source_audited":lt48_hc908_is_abasaz(r)?"hc908_abasaz_source_audited":lt48_hc908_is_gpgz(r)?"hc908_gpgz_source_audited":lt48_hc908_is_apsr(r)?"hc908_apsr_source_audited":lt48_hc908_is_grkx(r)?"hc908_grkx_source_audited":lt48_hc908_is_gxjb(r)?"hc908_gxjb_source_audited":lt48_hc908_is_jkjl(r)?"hc908_jkjl_source_audited":"hc908_source_audited";
                items[i].reason=lt48_hc908_is_fqtqy(r)?"F_QT/QY source MON08 entry and FPGA, original RAM/GPIO workflow audited; authored timed Flash helpers, both trims preserved and independent readback; hardware pending":lt48_hc908_is_gpgz(r)?"GP20/GZ original host, monitor/FPGA and explicit Flash-generation contracts; original workflow and independent effects audited; hardware pending":lt48_hc908_is_apsr(r)?"AP/SR original host and source entry/FPGA, exact sparse Flash geometry, manufacturer-timed RAM helpers and independent readback; hardware testing pending":lt48_hc908_is_gxjb(r)?"GP32/GT/JB/BD source monitor, exact profile memory/protection, FPGA/clock, RAM helper timing and independent verification; hardware pending":lt48_hc908_is_jkjl(r)?"JK/JL original host, source monitor entry/serial helpers, exact FPGA mapping and authored target RAM Flash helpers; sparse image and independent readback; hardware testing pending":"QT/QY source host, monitor workers and selected FPGA; authored target RAM helper with instruction-timed Flash writes, security check, both trim bytes preserved, independent readback; hardware testing pending";
            }
            lt48_hc908_close(r);return LT48_OK;
        }
    }
    if (entry.device_class == 2009u) {
        lt48_hitachi_resources *r=NULL;
        if(lt48_hitachi_open(installation,device_index,&r)==LT48_OK) {
            if(!lt48_hitachi_qualified(r)){lt48_hitachi_close(r);r=NULL;}
            else {
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++) {
                lt48_operation_kind op=items[i].operation;
                if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)continue;
                if(lt48_hitachi_is15_internal(r)&&op==LT48_OPERATION_ERASE){items[i].availability=LT48_OPERATION_UNAVAILABLE;items[i].reason="Original workflow has no Erase procedure; unavailable";continue;}
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level=lt48_hitachi_is15_internal(r)?"hitachi15_source_audited":lt48_hitachi_is_fujitsu(r)?"fujitsu41_source_audited":"hitachi_source_audited";
                items[i].reason=lt48_hitachi_is15_internal(r)?"Original H8 Program/Verify with authored raw Read matching source verification GPIO cycles; blank whole-image programming, independent readback; Erase unavailable; original adapter required; hardware pending":lt48_hitachi_is_fujitsu(r)?"Fujitsu source workflow, raw Read with canonical FF prefix, whole-image programming and erase, original margin verification and independent readback; original programming adapter required; hardware testing pending":"Exact Hitachi M1/M2 source applets, FPGA, P1 adapter map and voltage profile; 128-byte programming, whole-chip erase, original margin verification and independent readback; hardware testing pending";
            }
            lt48_hitachi_close(r);return LT48_OK;
            }
        }
    }
    if (entry.device_class == 2009u) {
        lt48_legacy_dataflash_resources *r=NULL;
        if(lt48_legacy_dataflash_open(installation,device_index,&r)==LT48_OK) {
            for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++) {
                lt48_operation_kind op=items[i].operation;
                if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)continue;
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level="legacy_dataflash_source_audited";
                items[i].reason="Exact DataFlash source applets, FPGA and pin map; nonbinary pages, preserving writes/FF erase, original margin verification and independent readback; hardware testing pending";
            }
            lt48_legacy_dataflash_close(r);return LT48_OK;
        }
    }
    if (entry.device_class == 2043u) {
        lt48_serial_page_resources *small=NULL;
        if(lt48_serial_page_open(installation,device_index,&small)==LT48_OK) {
            if(lt48_serial_page_is_lp21(small)) {
                for(size_t i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(items[i].operation<=LT48_OPERATION_CONFIGURATION && items[i].operation!=LT48_OPERATION_MEMORY_PROTECT) {
                    items[i].availability=LT48_OPERATION_EXECUTABLE;
                    items[i].support_level="at89_lp21_source_audited";
                    items[i].reason="Exact AT89LP213/214/216 source applets and FPGA; full-image code programming, erase/unlock, preserved fuses/trim/signature, explicit configuration and lock writes with independent readback; hardware qualification pending";
                }
                lt48_serial_page_close(small);return LT48_OK;
            }
            unsigned kind=lt48_serial_page_i2c_config_kind(small);
            int protected=(kind && kind!=4) || lt48_serial_page_validate(small,LT48_OPERATION_SECURE,0,lt48_serial_page_capacity(small),NULL,0)==LT48_OK;
            for(size_t i=0;i<(protected?8u:7u);i++) {
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level=!strcmp(lt48_installation_source_profile(installation),"AEC 14.40")?"aec1440_spi_source_audited":!strcmp(entry.vendor,"SUNPLUS")&&(entry.device_type==0xf3||entry.device_type==0xf4||entry.device_type==0xf5)?"sphe3_source_audited":kind==4?"at17n_source_audited":kind?"page_i2c_protected_source_audited":(entry.device_type>=0x5c && entry.device_type<=0x5e)?"serial_rram_source_audited":lt48_serial_page_is_nor(small)?"spi_nor_source_audited":protected?"spi_page_source_audited":"serial_page_source_audited";
                items[i].reason=kind==4?"Exact AT17N source applets/FPGA, preserving EEPROM writes and RESET/OE polarity configuration; hardware qualification pending":kind?"Source-bound I2C memory, explicit protection/configuration, preserved settings and independent readback; hardware testing pending":(entry.device_type>=0x5c && entry.device_type<=0x5e)?"Exact RM25C SPI resistive-memory procedure; full-chip source erase and independent margin/fresh-power readback; hardware testing pending":lt48_serial_page_is_nor(small)?"Exact SPI NOR source procedure; full-chip erase, verified status/protection, monotonic program, independent array/margin readback; hardware testing pending":protected?"Exact SPI EEPROM Page procedure; source status/protection handling, whole-array Secure, preserving FF clear and writes, margin/fresh-power readback; hardware testing pending":"Exact I2C Page procedure; preserving page writes, FF clear, independent readback and margin verification";
            }
            if(kind>=2){items[LT48_OPERATION_CONFIGURATION-1]=items[0];items[LT48_OPERATION_CONFIGURATION-1].operation=LT48_OPERATION_CONFIGURATION;}
        }
        lt48_serial_page_close(small);
        return LT48_OK;
    }
    if (entry.device_class == 2005u) {
        lt48_page_eeprom_resources *small = NULL;
        status = lt48_page_eeprom_open(installation, device_index, &small);
        if (status == LT48_OK) {
            for (size_t i = 0; i < 8; ++i) {
                items[i].availability = LT48_OPERATION_EXECUTABLE;
                if(i==LT48_OPERATION_SECURE-1u && lt48_page_eeprom_validate(small,LT48_OPERATION_SECURE,0,lt48_page_eeprom_capacity(small),NULL,0)!=LT48_OK) {
                    items[i].availability=LT48_OPERATION_UNAVAILABLE;
                    items[i].reason=entry.device_type==0xcu||entry.device_type==0xdu?"Standalone Winbond boot-block lockout deferred; ordinary Program uses the original protected page-write procedure":"Selected plain-write EEPROM procedure has no software Secure operation";
                    continue;
                }
                items[i].support_level = lt48_page_eeprom_is_word(small)?"at29word_source_audited":lt48_page6_member_internal(entry.device_class,entry.device_type)?"page6_source_audited":(entry.device_type==0xcu||entry.device_type==0xdu)?"winbond_page2_source_audited":entry.device_type==0xfffd?"cat28c512_native_procedure":"page_eeprom_source_audited";
                items[i].reason = (entry.device_type==0xcu||entry.device_type==0xdu)?"Winbond page flash: source protected page writes and explicit full-chip erase with independent full-array readback; standalone boot-block lockout deferred; hardware pending":"Exact selected page EEPROM engine; preserving partial writes, readback and margin verify; FF clear; software write protection where the selected procedure provides it";
            }
        }
        lt48_page_eeprom_close(small);
        return LT48_OK;
    }
    if(entry.device_class==2044u) {
        lt48_addtab_flash_resources *flash=NULL;
        if(lt48_addtab_flash_open(installation,device_index,&flash)==LT48_OK) {
            for(size_t i=0;i<7;i++) {
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                unsigned single=entry.device_type==0 || entry.device_type==1 || entry.device_type==4 || entry.device_type==5 || entry.device_type==9 || entry.device_type==10;
                items[i].support_level=single?"addtab_amd_mx6_source_audited":"addtab_byte8_source_audited";
                items[i].reason=single?"Byte-wide single-byte NOR: source regions, independent readback; DL316 erase includes secondary sector, LV065 erase preserves secondary data; original adapter required; hardware pending":"Byte-wide parallel NOR: original applets/FPGA, 32-byte page preservation, main-only erase and preserved SecSi; original adapter required; standalone protection deferred; hardware pending";
            }
            lt48_addtab_flash_close(flash);return LT48_OK;
        }
    }
    if (entry.device_class == 2044u || (entry.device_class == 2041u && !(page_byte21_batch(entry.device_class, entry.device_type)||(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type))))))) {
        lt48_addtab_spi_resources *flash=NULL;
        status=lt48_addtab_spi_open(installation,device_index,&flash);
        if(status==LT48_OK)for(size_t i=0;i<8;i++) {
            if(i==LT48_OPERATION_SECURE-1u && lt48_addtab_spi_validate(flash,LT48_OPERATION_SECURE,0,lt48_addtab_spi_capacity(flash),NULL,0)!=LT48_OK) {
                items[i].availability=LT48_OPERATION_UNAVAILABLE;
                items[i].reason="Selected original procedure has no Secure operation";
                continue;
            }
            items[i].availability=LT48_OPERATION_EXECUTABLE;
            items[i].support_level=!strcmp(lt48_installation_source_profile(installation),"AEC 14.40")?"aec1440_spi_source_audited":mr7_member(&entry)?"memreuse7_source_audited":lt48_addtab_spi_is_sstfwh_internal(flash)?"sstfwh1_source_audited":lt48_addtab_spi_is_df28_internal(flash)?"dataflash28ts1_source_audited":lt48_addtab_spi_is_sst49c7_internal(flash)?"sst49c7_source_audited":lt48_addtab_spi_is_spiw6_internal(flash)?"spiw6_source_audited":lt48_addtab_spi_is_spi26(flash)?"spi26_source_audited":lt48_addtab_spi_is_spi23(flash)?"spi23_source_audited":lt48_addtab_spi_is_spi54(flash)?"spi54_source_audited":lt48_addtab_spi_is_mac29(flash)?"spi_mac29_source_audited":lt48_addtab_spi_is_micron40(flash)?"spi_micron40_source_audited":!strcmp(lt48_installation_source_profile(installation),"AEC 14.30")?"aec1430_spi_source_audited":lt48_addtab_spi_memory_kind(flash)?"adesto_memory_source_audited":
                (lt48_addtab_spi_secure_is_permanent(flash) && entry.device_type>=0x7d && entry.device_type<=0x81)?"adesto_memory_source_audited":"addtab_spi_flash_source_audited";
            items[i].reason=lt48_addtab_spi_is_sstfwh_internal(flash)?"SST FWH: main memory plus24 OTP bytes, source sector unlock and raw Read; original PLCC32 adapter required; hardware pending":lt48_addtab_spi_is_sst49c7_internal(flash)?"SST LPC C-series: main array plus 24 user OTP bytes; Erase preserves OTP, Secure permanently locks OTP; original package adapter required; hardware pending":"Audited source-selected memory segments, page programming, erase/protection and independent readback; original package adapter required; hardware qualification pending";
        }
        lt48_addtab_spi_close(flash);
        return LT48_OK;
    }
    if (entry.device_class == 2045u || entry.device_class == 2047u) {
        lt48_addtab_flash_resources *flash=NULL;
        status=lt48_addtab_flash_open(installation,device_index,&flash);
        if(status==LT48_OK)for(size_t i=0;i<(lt48_addtab_flash_secure_kind(flash)?8u:7u);i++) {
            items[i].availability=LT48_OPERATION_EXECUTABLE;
            items[i].support_level=entry.device_class==2045u && addtab_mx26_sop_batch(entry.device_type)?"addtab_mx26_sop_source_audited":entry.device_class==2045u && addtab_st128_batch(entry.device_type)?"addtab_st128_source_audited":entry.device_class==2045u && addtab_samsung128_batch(entry.device_type)?"addtab_samsung128_source_audited":entry.device_class==2045u && addtab_d31six_batch(entry.device_type)?"addtab_d31six_source_audited":entry.device_class==2045u && addtab_spansion2_batch(entry.device_type)?"addtab_spansion2_source_audited":entry.device_class==2045u && addtab_eon_esmt15_batch(entry.device_type)?"addtab_eon_esmt15_source_audited":entry.device_class==2045u && addtab_samsung4_batch(entry.device_type)?"addtab_samsung4_source_audited":entry.device_class==2045u && addtab_remaining43_batch(entry.device_type)?"addtab_remaining43_source_audited":entry.device_class==2045u && addtab_exact17_batch(entry.device_type,entry.unknown6)?"addtab_exact17_source_audited":entry.device_class==2045u && addtab_vid6_batch(entry.device_type)?"addtab_vid6_source_audited":entry.device_class==2045u && addtab_winbond32_batch(entry.device_type)?"addtab_winbond32_source_audited":entry.device_class==2045u && addtab_mx26_batch(entry.device_type)?"addtab_mx26_source_audited":entry.device_class==2045u && addtab_sst7_batch(entry.device_type)?"addtab_sst7_source_audited":entry.device_class==2045u && addtab_sanyo5_batch(entry.device_type)?"addtab_sanyo5_source_audited":entry.device_class==2045u && addtab_tail9_batch(entry.device_type)?"addtab_tail9_source_audited":entry.device_class==2045u && addtab_tail24_batch(entry.device_type)?"addtab_tail24_source_audited":entry.device_class==2045u && addtab_tail18_batch(entry.device_type)?"addtab_tail18_source_audited":entry.device_class==2047u?"addtab_otp_source_audited":addtab_dl46_batch(entry.device_type)?"addtab_shared_dl46_source_audited":addtab_glp34_batch(entry.device_type)?"addtab_glp34_source_audited":addtab_gln20_batch(entry.device_type)?"addtab_gln20_source_audited":addtab_gln86_batch(entry.device_type)?"addtab_gln86_source_audited":addtab_word47_batch(entry.device_type)?"addtab_word47_source_audited":addtab_glm22_batch(entry.device_type)?"addtab_glm22_source_audited":addtab_gl47_batch(entry.device_type)?"addtab_gl47_source_audited":addtab_gl57_batch(entry.device_type)?"addtab_gl57_source_audited":addtab_gl45_batch(entry.device_type)?"addtab_shared_gl45_source_audited":addtab_ew18_batch(entry.device_type)?"addtab_ew18_source_audited":addtab_micron27_batch(entry.device_type)?"addtab_micron27_source_audited":addtab_ew54_batch(entry.device_type)?"addtab_ew54_source_audited":addtab_gl84_batch(entry.device_type)?"addtab_gl84_source_audited":addtab_gl67_batch(entry.device_type)?"addtab_gl67_source_audited":addtab_dl54_batch(entry.device_type)?"addtab_dl54_source_audited":addtab_word131_batch(entry.device_type)?"addtab_word131_source_audited":((entry.device_type>=0x1e && entry.device_type<=0x23) || entry.device_type==0x2c || entry.device_type==0x2d || entry.device_type==0x2e || entry.device_type==0x32 || entry.device_type==0x33)?"flash_near79_source_audited":"addtab_amd_flash_source_audited";
            items[i].reason=entry.device_class==2047u?"Exact Sharp/ST AddTab mapped-array and OTP procedures, erase preservation, independent readback and definition-selected security; Secure includes permanent locks; original adapter required; hardware qualification pending":addtab_dl46_batch(entry.device_type)?"Source-audited DL main/special regions, complete bank ID tables and erase prerequisites; seven array operations; standalone Secure deferred; original adapter required; hardware pending":addtab_glp34_batch(entry.device_type)?"Exact GL-P 32-word programming, four-byte address, preserved extra region, original protection where present and NewDeviceLib partial-range correction; original adapter required; hardware pending":(addtab_gln20_batch(entry.device_type)||addtab_gln86_batch(entry.device_type))?"Exact GL-N 16-word programming with four-byte address, preserved special region, source-specific protection and two FPGA bindings; original adapter required; hardware pending":addtab_word47_batch(entry.device_type)?"Exact word-NOR host, alternate-unlock erase applet, per-definition main/special erase bounds and original FPGA; original adapter required; hardware pending":(addtab_glm22_batch(entry.device_type)||addtab_gl47_batch(entry.device_type))?"Exact GL host and applet reuse, audited FPGA binding, 16-word programming with preservation, main-only erase and protection; original adapter required; hardware pending":addtab_gl57_batch(entry.device_type)?"Exact 16-word GL programming, preserved special region and source-defined PPB status normalization where available; original adapter required; hardware qualification pending":addtab_gl45_batch(entry.device_type)?"Exact shared GL source procedures, 16-word programming and preserved special region; main-array protection only where present; original adapter required; hardware pending":(addtab_ew18_batch(entry.device_type)||addtab_micron27_batch(entry.device_type))?"M29W/M29EW source 256-word 33/29 programming, source main-only erase and preserved special region; authored partial-range address restoration; original adapter required; hardware pending":addtab_ew54_batch(entry.device_type)?"M29EW/GL-S source-audited main and special regions, 256/512-word pages with preservation and source UNLOCK; NewDeviceLib region transitions; no Secure operation; original adapter required; hardware pending":addtab_gl84_batch(entry.device_type)?"Exact GL main/special regions, 64-byte programming with partial-page preservation, source erase geometry and independent readback; source has no Secure operation; original package adapter required; hardware qualification pending":addtab_gl67_batch(entry.device_type)?"Exact 16-word GL programming, source main-array erase/protection geometry and preserved 256-byte special region; only source-present operations enabled; original adapter required; hardware qualification pending":"Exact source segments, erase/protection geometry, special-region switching and independent array/status readback; Secure covers main array; permanent special-region locking excluded; original package adapter required; hardware qualification pending";
        }
        lt48_addtab_flash_close(flash);
        return LT48_OK;
    }
    if (lt48_nxp18_member_internal(entry.device_class,entry.device_type) || entry.device_class == 2032u || entry.device_class == 2030u || entry.device_class == 2031u || entry.device_class == 2033u || entry.device_class==2042u || (page_byte21_batch(entry.device_class,entry.device_type)||(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type)))))) {
        lt48_xp_flash_resources *flash=NULL;
        status=lt48_xp_flash_open(installation,device_index,&flash);
        if(status==LT48_OK)for(size_t i=0;i<(lt48_xp_flash_secure_kind(flash)?8u:7u);i++) {
            items[i].availability=LT48_OPERATION_EXECUTABLE;
            items[i].support_level=lv800three_member(&entry)?"lv800three_source_audited":(entry.device_class==2030u&&entry.device_type==0xbdu&&mr7_member(&entry))?"lv088three_source_audited":mr7_member(&entry)?"memreuse7_source_audited":tmp1962one_member(&entry)?"tmp1962one_source_audited":lt48_lpc16_member_internal(entry.device_class,entry.device_type)?"lpc16_source_audited":nand2_binding(&entry)?"nand2_source_audited":nand15_binding(&entry)?"nand15_source_audited":emtc18_binding(&entry)?"emtc18_source_audited":lt48_nxp18_member_internal(entry.device_class,entry.device_type)?"nxp18_source_audited":lt48_sharp13_member_internal(entry.device_class,entry.device_type)?"sharp13_source_audited":lt48_parallel20_member_internal(entry.device_class,entry.device_type)?"parallel20_source_audited":(entry.device_class==2001u&&entry.device_type>=0x3c&&entry.device_type<=0x40)?"novatek_umc6_source_audited":(entry.device_class==2001u&&(entry.device_type==0x25||entry.device_type==0x3a||entry.device_type==0x3b))?"eprom3_source_audited":at29_binding(&entry)?"at29_13_source_audited":green14_binding(&entry)?"green14_source_audited":(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type))))?"sst_page36_source_audited":(entry.device_class==2031u && entry.device_type>=0x107 && entry.device_type<=0x10a)?"xp_atmel64_source_audited":(entry.device_class==2033u && xp_sanyo4_batch(entry.device_type))?"xp_sanyo4_source_audited":(entry.device_class==2033u && xp_seiko12_batch(entry.device_type))?"xp_seiko12_source_audited":(entry.device_class==2033u && entry.device_type>=0x37 && entry.device_type<=0x39)?"st_m36wwr3_source_audited":(entry.device_class==2033u && xp_intel29_batch(entry.device_type))?"xp_intel29_source_audited":(entry.device_class==2030u && (entry.device_type==0x88 || entry.device_type==0x8c || entry.device_type==0x8e || entry.device_type==0x90 || entry.device_type==0x92 || (entry.device_type>=0x99 && entry.device_type<=0xa5)))?"xp_tail34_source_audited":(entry.device_class==2030u && (entry.device_type==0x85 || entry.device_type==0x86 || entry.device_type==0x87 || entry.device_type==0x89 || entry.device_type==0x8a || entry.device_type==0x8b || entry.device_type==0x8d || entry.device_type==0x8f || entry.device_type==0x91 || entry.device_type==0x93 || entry.device_type==0x95 || entry.device_type==0x96 || entry.device_type==0x97 || entry.device_type==0x98))?"xp_lpc29_source_audited":(entry.device_class==2032u && entry.device_type==0x5f)?"xp_sharp1305_source_audited":(entry.device_class==2032u && entry.device_type==8)?"xp_ice2_source_audited":(entry.device_class==2032u && entry.device_type>=4 && entry.device_type<=7)?"xp_amd28fa_source_audited":(entry.device_class==2032u && (entry.device_type==0x11 || entry.device_type==0x13 || entry.device_type==0x15 || entry.device_type==0x16 || entry.device_type==0x18 || entry.device_type==0x1f || entry.device_type==0x20 || entry.device_type==0x21 || entry.device_type==0x22))?"xp_mux15_source_audited":(entry.device_class==2032u && (entry.device_type==0x9 || entry.device_type==0xa || entry.device_type==0xb || entry.device_type==0xc || entry.device_type==0xd || entry.device_type==0xf || entry.device_type==0x10 || entry.device_type==0x12 || entry.device_type==0x14 || entry.device_type==0x17 || entry.device_type==0x1c || entry.device_type==0x1d))?"xp_mux30_source_audited":(entry.device_class==2032u && (entry.device_type==0xe || entry.device_type==0x19 || entry.device_type==0x1a || entry.device_type==0x1b || entry.device_type==0x1e))?"xp_mux12_source_audited":(entry.device_class==2031u && (entry.device_type==0x136 || (entry.device_type>=0x10c && entry.device_type<=0x10e)))?"xp_st102_source_audited":(entry.device_class==2033u && entry.device_type>=0xbc && entry.device_type<=0xc1)?"xp_atmel_bga6_source_audited":(entry.device_class==2032u && entry.device_type>=0x62 && entry.device_type<=0x68)?"xp_sanyo7_source_audited":(entry.device_class==2031u && (entry.device_type==0xfd || entry.device_type==0xff || entry.device_type==0x100))?"xp_atmel7_source_audited":(entry.device_class==2031u && xp_amic24_batch(entry.device_type))?"xp_amic24_source_audited":(entry.device_class==2031u && (entry.device_type==0x29 || entry.device_type==0x2a || entry.device_type==0x2f || entry.device_type==0x30))?"xp_macronix_kh12_source_audited":(entry.device_class==2031u && xp_atmel_sanyo15_batch(entry.device_type))?"xp_atmel_sanyo15_source_audited":(entry.device_class==2031u && entry.device_type>=0xdd && entry.device_type<=0xe8)?"xp_macronix_uti20_source_audited":(entry.device_class==2032u && entry.device_type<=3u)?"xp_legacy28f18_source_audited":(page_byte21_batch(entry.device_class,entry.device_type)||(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type)))))?"intel_page_byte21_source_audited":(entry.device_class==2033u && intel_shared_word118_batch(entry.device_type))?"intel_shared_word118_source_audited":(entry.device_class==2033u && intel_word100_batch(entry.device_type))?"intel_word100_source_audited":entry.device_class==2032u?(intel_shared_byte58_batch(entry.device_type)?"intel_shared_byte58_source_audited":"intel_byte58_source_audited"):entry.device_class==2030u?(xp_shared_byte75_batch(entry.device_type)?"xp_shared_byte75_source_audited":xp_byte60_batch(entry.device_type)?"xp_byte60_source_audited":xp_byte128_batch(entry.device_type)?"xp_byte126_source_audited":"xp_byte155_source_audited"):(entry.device_class==2031u && xp_shared_word_batch(entry.device_type))?"xp_shared_word168_source_audited":(entry.device_class==2031u && xp_word63_batch(entry.device_type))?"xp_word63_source_audited":(entry.device_class==2031u && xp_word138_batch(entry.device_type))?"xp_word138_source_audited":(entry.device_class==2031u && xp_so44_batch(entry.device_type))?"xp_so44_source_audited":entry.device_class==2042u?"page_wide_source_audited":(entry.device_class==2031u && (entry.device_type==0x2d || entry.device_type==0x2e || entry.device_type==0x33 || entry.device_type==0x34))?"flash_near79_source_audited":(entry.device_class==2031u && (entry.device_type==0x10 || entry.device_type==0x11 || entry.device_type==0x16 || entry.device_type==0x17 || entry.device_type==0x5c || entry.device_type==0x5d))?"amd_lv_source_audited":entry.device_class==2033u?(entry.device_type>=0xb0u && entry.device_type<=0xbbu?"at17f_source_audited":"c3_flash_source_audited"):(entry.device_type>=0xa7u && entry.device_type<=0xb0u)?"en29lv_source_audited":"xp_flash_source_audited";
            items[i].reason=lv800three_member(&entry)?"Source-audited LV800 word NOR, raw Read, whole-chip erase and independent readback; no standalone protection; original rotated TSOP48 adapter required; hardware pending":emtc18_binding(&entry)?"EMTC page memory: raw Read and source page operations; protected boot blocks refuse mutation; standalone protection/configuration unavailable; original adapter required; hardware pending":(entry.device_class==2031u && (entry.device_type==0xb4 || entry.device_type==0x5b))?"Source-audited NOR main-array operations, exact source erase/protection geometry and independent readback; standalone protection deferred; source adapter required; hardware pending":at29_binding(&entry)?"Atmel parallel page flash; source page sizes, software protection and optional permanent boot locks; raw Read and independent readback; hardware pending":(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type))))?"SST-compatible 128-byte page memory; rewriting, source erase and software protection; independent readback; source adapter required; hardware pending":(entry.device_class==2033u && xp_sanyo4_batch(entry.device_type))?"Source-audited SANYO word NOR, seven-read unlock and 256-byte sector erase; original adapter required; hardware pending":(entry.device_class==2033u && xp_seiko12_batch(entry.device_type))?"Source-audited Seiko word NOR and permanent protection-register locks with data preservation; original TSOP adapter required; hardware pending":(entry.device_class==2033u && entry.device_type>=0x37 && entry.device_type<=0x39)?"Source-audited ST M36 word NOR, exact boot sectors and addressed or paired identity; Secure permanently locks registers, preserving register data; original adapter required; hardware pending":(entry.device_class==2033u && xp_intel29_batch(entry.device_type))?"Source-audited Intel word NOR, both-die identity checks and preserved OTP register data; Secure permanently locks registers; original adapter required; hardware pending":(entry.device_class==2031u && (entry.device_type==0x29 || entry.device_type==0x2a || entry.device_type==0x2f || entry.device_type==0x30))?"Exact Macronix KH/MX word-NOR setup, whole-array erase and sector protection; independent readback; source adapter required; hardware pending":(entry.device_class==2031u && xp_atmel_sanyo15_batch(entry.device_type))?"Source-audited Atmel/Sanyo main-array workflows, primary sector erase and source unprotect prerequisite; standalone protection deferred; original adapter required; hardware pending":(entry.device_class==2031u && entry.device_type>=0xdd && entry.device_type<=0xe8)?"Source-audited Macronix/UTI word NOR, primary-sector erase and margin readback; UTI source has no device-ID procedure; source adapter required; hardware pending":(entry.device_class==2032u && entry.device_type<=3u)?"Source-audited legacy 28F: full-array zero prefill before erase, bounded pulse/verify retry loop, margins and fresh readback; hardware pending":(page_byte21_batch(entry.device_class,entry.device_type)||(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type)))))?"Exact Intel byte-Page procedures, 32-byte pages, source block erase/unlock and margin readback; original adapter required; hardware pending":(entry.device_class==2033u && intel_shared_word118_batch(entry.device_type))?"Source-audited Intel/Sharp main-array workflows, source-specific pin defaults and sector geometry; standalone protection deferred; original adapter required; hardware pending":(entry.device_class==2033u && intel_word100_batch(entry.device_type))?"Source-audited Intel-style word NOR, original geometry and voltage margins, independent readback; optional permanent register locks preserve existing data; exact ST lock-write correction in NewDeviceLib; original adapter required, hardware qualification pending":entry.device_class==2032u?"Exact Intel byte-NOR procedures, mixed boot-sector erase geometry, margin verification and independent array readback; standalone Secure deferred; original adapter required; hardware qualification pending":entry.device_class==2030u?"Exact source byte-wide NOR procedures, voltage margins and independent readback; protection is device-specific and may be permanent; use the source adapter; hardware qualification pending":entry.device_class==2042u?"Exact Page wide-flash procedures, source page sizes, sector geometry, voltage margins and independent readback; Secure scope is definition-specific (array or permanent OTP register locks); source adapter required; hardware qualification pending":entry.device_class==2033u && entry.device_type<0xb0u?"C3 main array: exact source procedures, primary sector unlock/erase, margin verification and independent readback; special protection-register programming/locking unavailable; original package adapter required; hardware qualification pending":entry.device_class==2033u?"Exact AT17F word-stream procedures, whole-array erase and independent readback; use the original package adapter; audit only, hardware qualification pending":"Exact XP word-flash source procedures, sector protection/unprotection and independent array/status readback; use the original package adapter; hardware qualification remains sample-specific";
        }
        if(nand15_binding(&entry))for(size_t i=0;i<7;i++)items[i].reason="Small-page NAND physical 512+16-byte pages; raw Read, no ECC/remapping; bad-block mutations refused; whole images only; original adapter; hardware pending";
        lt48_xp_flash_close(flash);
        return LT48_OK;
    }
    if ((entry.device_class == 2034u && !(entry.device_type>=0x2a && entry.device_type<=0x2f)) || entry.device_class==2038u) {
        lt48_xp_eprom_resources *small = NULL;
        status = lt48_xp_eprom_open(installation, device_index, &small);
        if(status==LT48_OK && entry.device_class==2038u && entry.device_type>=0x61&&entry.device_type<=0x70&&!lt48_w78legacy90_admitted_internal(entry.device_type)){lt48_xp_eprom_close(small);return LT48_OK;}
        if(status==LT48_OK&&w78e354one_member(&entry)&&!LT48_W78E354ONE_ADMITTED){lt48_xp_eprom_close(small);return LT48_OK;}
        int electrical=status==LT48_OK && entry.device_class==2034u && lt48_xp_eprom_validate(small,LT48_OPERATION_ERASE,0,lt48_xp_eprom_capacity(small),NULL,0)==LT48_OK;
        if (status == LT48_OK) {
            for (size_t i = 0; i < (entry.device_class==2038u?(entry.device_type>=0x72&&entry.device_type<=0x75?7u:8u):electrical?7u:6u); ++i) {
                items[i].availability = LT48_OPERATION_EXECUTABLE;
                items[i].support_level = entry.device_class==2038u?(entry.device_type>=0x72&&entry.device_type<=0x75?"w78eight_source_audited":entry.device_type>=0x61&&entry.device_type<=0x70?"w78legacy90_source_audited":lt48_xp_eprom_extended_fields(small)?"xp_philips_extended_source_audited":lt48_xp_eprom_is_ra2(small)?"xp_philips_ra2_source_audited":lt48_xp_eprom_is_philips(small)?(lt48_xp_eprom_flag_fields(small)==4?"xp_philips_x2_source_audited":"xp_philips_basic_source_audited"):lt48_xp_eprom_w78_fields(small)?(lt48_xp_eprom_w78_fields(small)==6?"xp_w78d_source_audited":"xp_w78_source_audited"):lt48_xp_eprom_is_rb2(small)?"at89_rb2_source_audited":lt48_xp_eprom_has_configuration(small)?"at89_parallel_source_audited":"at89_classic_source_audited"):electrical?"ptc_xp4_source_audited":lt48_xp_eprom_is_fram(small)?"parallel_fram_source_audited":entry.device_type<=1?"st_m27w_source_audited":"hitachi_xp_source_audited";
                items[i].reason = entry.device_class==2038u?(entry.device_type>=0x72&&entry.device_type<=0x75?"W78E37x: raw code Read and seven operations; Program requires blank, Overprogram permits only1-to0; explicit Erase; no source Secure or Configuration; original adapter required; hardware pending":lt48_xp_eprom_is_ra2(small)?"Philips/NXP RA2: complete code operations; preserve clock/status/boot settings during programming; configuration readback checked; source audited, hardware pending":lt48_xp_eprom_is_philips(small)?"Philips/NXP: complete code operations and source configuration; protected parts erased before programming; source audited, hardware pending":lt48_xp_eprom_w78_fields(small)?"W78: full-image Program/Overprogram erase first; independent readback; explicit configuration and preserved non-protection settings; source audited, hardware pending":lt48_xp_eprom_has_configuration(small)?"AT89 parallel: protection readback, source conditional erase for locked parts, complete code operations; audit only, hardware pending":"AT89 classic: full-image Program/Overprogram automatically erases; independent readback; three explicit lock levels; source audit only, hardware pending"):electrical?"PTC source erase pulse, original XP applets and FPGA; independent full-array readback; original adapter required; hardware pending":lt48_xp_eprom_is_fram(small)?"Exact Dataman parallel FRAM procedure; native C random overwrite, nominal-relative margin verify; no chip erase opcode":entry.device_type<=1?"ST M27W exact source applets, FPGA and XP byte-stream host contract; audit basis; hardware pending":"Exact Dataman Hitachi XP applets and host block-transfer contract audited; native C owner; physical testing pending";
            }
        }
        if(status==LT48_OK&&w78e354one_member(&entry)){for(unsigned i=0;i<7;i++){items[i].support_level="w78e354one_source_audited";items[i].reason="W78E354 source array procedures; raw Read, blank Program/1-to0 Overprogram, explicit Erase, preserved watchdog/supervisor option byte and ROM-MAP, independent margin and fresh-power readback; adapter/hardware pending";}items[7].availability=LT48_OPERATION_UNAVAILABLE;items[7].reason="Watchdog/supervisor options are preserved; no lock or option editor exposed";}
        if(status==LT48_OK&&sierra2_member(&entry))for(unsigned i=0;i<7;i++){items[i].support_level="sierra2_source_audited";items[i].reason="Sierra multiplexed EEPROM: raw Read and arbitrary rewrites; Erase is verified FF fill; original adapter; hardware pending";}
        if(status==LT48_OK&&ptc010three_member(&entry))for(unsigned i=0;i<7;i++){items[i].support_level="ptc010three_source_audited";items[i].reason="PTC electrically erasable memory; exact600/420cV source verification; raw Read and independent erase/program readback; original adapter; hardware pending";}
        if(status==LT48_OK&&gdacotp6_catalogue_member(&entry))for(unsigned i=0;i<6;i++){items[i].support_level="gdacotp6_source_audited";items[i].reason="EPROM/OTP: source applet/FPGA and voltage profile; full raw Read, blank-before-Program and1-to0 Overprogram; no electrical Erase; original adapter; hardware pending";}
        if(status==LT48_OK&&amic4_catalogue_member(&entry))for(unsigned i=0;i<6;i++){items[i].support_level="amic4_source_audited";items[i].reason="AMIC OTP EPROM: source retry/pulse procedure, blank-before-Program,1-to0 Overprogram, raw Read and independent verification; no electrical Erase; original adapter; hardware pending";}
        if(status==LT48_OK && lt48_xp_eprom_has_configuration(small)) {
            items[9].availability=LT48_OPERATION_EXECUTABLE;
            items[9].support_level=entry.device_class==2038u&&entry.device_type>=0x61&&entry.device_type<=0x70?"w78legacy90_source_audited":lt48_xp_eprom_extended_fields(small)?"xp_philips_extended_source_audited":lt48_xp_eprom_is_ra2(small)?"xp_philips_ra2_source_audited":lt48_xp_eprom_is_philips(small)?(lt48_xp_eprom_flag_fields(small)==4?"xp_philips_x2_source_audited":"xp_philips_basic_source_audited"):lt48_xp_eprom_w78_fields(small)?(lt48_xp_eprom_w78_fields(small)==6?"xp_w78d_source_audited":"xp_w78_source_audited"):lt48_xp_eprom_is_rb2(small)?"at89_rb2_source_audited":"at89_parallel_source_audited";
            items[9].reason=lt48_xp_eprom_is_ra2(small)?"Read/edit clock, lock level, status and boot vector; preserve unselected fields and verify readback":lt48_xp_eprom_is_philips(small)?"Read/edit source lock/configuration flags; preserve unselected flags and verify readback; clearing requires erase":lt48_xp_eprom_w78_fields(small)?"Read/edit source-defined W78 configuration bits; preserve unselected settings and verify readback":lt48_xp_eprom_is_rb2(small)?"Read or edit seven boot/protection fields; preserve unselected settings and verify readback":"Read current protection level (0..3); this action does not change configuration";
        }
        if(status==LT48_OK)
            if(entry.device_class!=2038u && !electrical)items[6].reason=lt48_xp_eprom_is_fram(small)?"FRAM has no erase command; Program an FF image to clear the array":"UV EPROM: electrical erase is not applicable";
        lt48_xp_eprom_close(small);
        return LT48_OK;
    }
    if ((entry.device_class==2010u&&!strcmp(entry.vendor,"Hyundai(Hynix)")) || (entry.device_class == 2013u && (st62_owner || entry.device_type==0xc5 || !strcmp(entry.vendor,"Zilog") || !strcmp(entry.vendor,"ELAN"))) || entry.device_class == 2012u || entry.device_class == 2011u || entry.device_class == 2000u || (entry.device_class==2034u && (entry.device_type>=0x2a && entry.device_type<=0x2f))) {
        lt48_small_eprom_resources *small = NULL;
        status = lt48_small_eprom_open(installation, device_index, &small);
        if (status == LT48_OK) {
            for (size_t i = 0; i < ((lt48_small_eprom_is_syntek4(small)||lt48_small_eprom_is_scenix6(small)||lt48_small_eprom_is_hyundai13_internal(small)||lt48_small_eprom_is_hyundai18_internal(small)||lt48_small_eprom_is_hyundai25_internal(small)||lt48_small_eprom_is_cop48(small)||lt48_small_eprom_is_compact40(small)||lt48_small_eprom_is_at89(small)||lt48_small_eprom_is_cypress55(small)||lt48_small_eprom_is_zilog70(small)||lt48_small_eprom_is_st62(small))?8u:(lt48_small_eprom_is_eeprom(small)||lt48_small_eprom_is_sram(small)||lt48_small_eprom_is_timekeeper(small))?7u:6u); ++i) {
                if((lt48_small_eprom_is_syntek4(small)||lt48_small_eprom_is_hyundai13_internal(small)||lt48_small_eprom_is_hyundai18_internal(small)||lt48_small_eprom_is_hyundai25_internal(small)||lt48_small_eprom_is_cypress55(small)||lt48_small_eprom_is_zilog70(small)||lt48_small_eprom_is_st62(small))&&items[i].operation==LT48_OPERATION_ERASE)continue;
                if(lt48_small_eprom_is_hc711_parallel(small)&&items[i].operation==LT48_OPERATION_OVER_PROGRAM)continue;
                if((lt48_small_eprom_is_syntek4(small)||lt48_small_eprom_is_hyundai13_internal(small)||lt48_small_eprom_is_cop48(small)||lt48_small_eprom_is_compact40(small))&&(items[i].operation==LT48_OPERATION_ERASE||items[i].operation==LT48_OPERATION_SECURE)&&lt48_small_eprom_validate(small,items[i].operation,0,lt48_small_eprom_capacity(small),NULL,0)!=LT48_OK)continue;
                items[i].availability = LT48_OPERATION_EXECUTABLE;
                items[i].support_level = lt48_small_eprom_is_elan5(small)?"elan5_source_audited":lt48_small_eprom_is_syntek4(small)?"syntek4_source_audited":lt48_small_eprom_is_scenix6(small)?"scenix6_source_audited":lt48_small_eprom_is_hyundai13_internal(small)?"hyundai13_source_audited":lt48_small_eprom_is_hyundai18_internal(small)?"hyundai18_source_audited":lt48_small_eprom_is_hyundai25_internal(small)?"hyundai25_source_audited":entry.device_class==2000u && entry.device_type==0xf5u?"atmel_hc2_source_audited": entry.device_class==2000u&&(entry.device_type==0x188||entry.device_type==0xf6||entry.device_type==0xbf)?"compact_prom3_source_audited":entry.device_class==2011u&&entry.device_type<=6u&&!strcmp(entry.vendor,"Cypress")?"cypress_prom11_source_audited":lt48_small_eprom_is_holtek19(small)?"holtek19_source_audited":lt48_small_eprom_is_cop48(small)?"cop48_source_audited":lt48_small_eprom_is_compact40(small)?"compact40_source_audited":lt48_small_eprom_is_st62(small)?"st62_source_audited":lt48_small_eprom_is_z8tail(small)?"z8tail_source_audited":lt48_small_eprom_is_zilog70(small)?"zilog_otp_source_audited":lt48_small_eprom_is_cypress55(small)?"cypress_usb_otp_source_audited":lt48_small_eprom_is_mcm68766(small)?"mcm68766_source_audited":lt48_small_eprom_is_hc711_parallel(small)?"hc711_parallel_source_audited":lt48_small_eprom_is_at89(small)?"at89_compact_source_audited":lt48_small_eprom_is_timekeeper(small)?"timekeeper_native_audited":lt48_small_eprom_is_sram(small)?"battery_sram_source_audited":lt48_small_eprom_is_eeprom(small)?"compact_parallel_eeprom_source_audited":lt48_small_eprom_is_nvram(small)?"parallel_nvram_source_audited":entry.device_class==2000u?"philips_prom_source_audited":"small_eprom_source_audited";
                items[i].reason = lt48_small_eprom_is_at89(small)?"Exact compact AT89 code/erase/lock procedures with internal counter addressing; X2 code writes erase the full chip; hardware pending":lt48_small_eprom_is_nvram(small)?"Source-bound compact SRAM/STORE/RECALL procedure and fresh-power persistence verification; clear with FF Program":"Exact Dataman compact memory procedures and host range contract audited; native C owner; physical testing pending";
            }
        }
        if(status==LT48_OK&&lt48_small_eprom_is_elan5(small)){items[9].availability=LT48_OPERATION_EXECUTABLE;items[9].support_level="elan5_source_audited";items[9].reason="Read separate configuration words or clear selected OTP bits; preserve code and other settings";}
        if(status==LT48_OK&&lt48_small_eprom_is_syntek4(small)){items[9].availability=LT48_OPERATION_EXECUTABLE;items[9].support_level="syntek4_source_audited";items[9].reason="Read separate options or clear selected OTP bits; preserve other choices and verify readback";}
        if(status==LT48_OK&&lt48_small_eprom_is_scenix6(small)){items[9].availability=LT48_OPERATION_EXECUTABLE;items[9].support_level="scenix6_source_audited";items[9].reason="Read FUSE/FUSEX or clear selected user bits; factory trim and BOR retained";}
        if(status==LT48_OK&&lt48_small_eprom_hyundai13_option_mask(small)){items[9].availability=LT48_OPERATION_EXECUTABLE;items[9].support_level="hyundai13_source_audited";items[9].reason="Read actual configuration or set selected OTP bits, preserving all others and checking raw readback";}
        if(status==LT48_OK&&lt48_small_eprom_st62_option_mask(small)){
            items[9].availability=LT48_OPERATION_EXECUTABLE;items[9].support_level="st62_source_audited";
            items[9].reason="Read decoded source options or permanently set selected bits; preserve unselected options; independently re-read after power removal";
        }
        if(status==LT48_OK && lt48_small_eprom_is_zilog_word(small)){
            items[9].availability=LT48_OPERATION_EXECUTABLE;items[9].support_level="zilog_otp_source_audited";
            items[9].reason="Read or irreversibly program nine source option fields; preserve unselected and reserved OTP cells; verify after power removal";
        }
        if(status==LT48_OK && lt48_small_eprom_is_timekeeper(small)) {
            for(unsigned i=0;i<LT48_OPERATION_KIND_COUNT;i++)if(i<7 || i==LT48_OPERATION_CONFIGURATION-1){
                items[i].availability=LT48_OPERATION_EXECUTABLE;
                items[i].support_level="timekeeper_native_audited";
                items[i].reason="RTC-aware RAM backup/restore/FF clear; RAM comparisons exclude clock registers; explicit clock configuration; original byte bus plus manufacturer register semantics; hardware pending";
            }
        }
        if(status==LT48_OK && lt48_small_eprom_is_nvram(small))
            items[6].reason="Clear the nonvolatile array by programming an all-FF image; STORE and retention verification are included";
        if(status==LT48_OK && entry.device_class==2000u)
            items[6].reason=lt48_small_eprom_is_mcm68766(small)?"UV EPROM: use external ultraviolet erase; no electrical erase or Secure":"One-time programmable bipolar PROM: erase is not applicable";
        lt48_small_eprom_close(small);
        return LT48_OK;
    }
    if (entry.device_class != 2001u && entry.device_class != 2006u &&
        entry.device_class != 2009u && entry.device_class != 2003u && entry.device_class != 2004u) return LT48_OK;

    status = lt48_m37_resources_open(installation, device_index, &resources);
    if (status != LT48_OK) {
        items[0].reason = "definition fails the digest-locked byte-stream Read resource gate";
        items[1].reason = items[0].reason;
        items[4].reason = items[0].reason;
        return LT48_OK;
    }
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL || audit->capacity_bytes == 0u) {
        lt48_m37_resources_close(resources);
        return LT48_OK;
    }
    status = lt48_m37_resources_build_offline_read_plan(
        resources, 0u, audit->capacity_bytes, &plan
    );
    if (status == LT48_OK) {
        items[0].availability = audit->read_behavior_approved ?
            LT48_OPERATION_EXECUTABLE : LT48_OPERATION_PLAN_ONLY;
        items[0].support_level = audit->read_original_replay_verified ?
            "original_windows_replay_verified" : audit->read_behavior_approved ?
            "engine_hardware_verified" : audit->read_forward_reference_verified ?
            "original_read_forward_verified" : "workflow_reconstructed";
        items[0].reason = audit->read_original_replay_verified ?
            "complete original Windows success-path and native C cleanup replay are enabled for this exact Read procedure; fault-branch equivalence and mutating operations are not implied" :
            audit->read_behavior_approved ?
            "digest-locked 27128 read plan, execution and cleanup are enabled" :
            audit->read_forward_reference_verified ?
            "Read forward procedure matches complete original execution; recovery qualification is incomplete and live execution is disabled" :
            "digest-locked reconstructed Read and cleanup plans; original Windows capture and representative hardware evidence are missing";
    }
    lt48_protocol_plan_close(plan);
    plan = NULL;
    if (audit->capacity_bytes != 0x4000u &&
        !audit->complete_group_equivalence_verified &&
        !audit->complete_source_audit_verified) {
        lt48_m37_resources_close(resources);
        return LT48_OK;
    }

    probe_length=(entry.device_class==2003u || entry.device_class==2004u)?2u:1u;
    status = lt48_m37_resources_build_blank_check_plan(
        resources, 0u, probe_length, &plan
    );
    if (status == LT48_OK) {
        items[1].availability = LT48_OPERATION_EXECUTABLE;
        items[1].support_level = "engine_hardware_verified";
        items[1].reason = "W14 success and mismatch results plus powered cleanup are hardware-validated";
    }
    lt48_protocol_plan_close(plan);
    plan = NULL;
    status = lt48_m37_program_plan_set_create(
        resources, 0u, verify_byte, probe_length, &program_set
    );
    if (status == LT48_OK) {
        items[2].availability = LT48_OPERATION_EXECUTABLE;
        items[2].support_level = "engine_hardware_verified";
        items[2].reason =
            "blank precheck, mode-2 programming, and mandatory readback are enabled as separate powered cycles";
        items[3].availability = LT48_OPERATION_EXECUTABLE;
        items[3].support_level = "engine_hardware_verified";
        items[3].reason =
            "host read proves every requested transition is one-to-zero before programming and mandatory readback";
    }
    lt48_m37_program_plan_set_close(program_set);
    status = lt48_m37_resources_build_verify_plan(
        resources, 0u, verify_byte, probe_length, &plan
    );
    if (status == LT48_OK) {
        items[4].availability = LT48_OPERATION_EXECUTABLE;
        items[4].support_level = "engine_hardware_verified";
        items[4].reason = "W16 success and mismatch results plus low/nominal/high voltage passes are hardware-validated";
    }
    lt48_protocol_plan_close(plan);
    plan = NULL;
    status = lt48_m37_resources_build_compare_plan(
        resources, 0u, verify_byte, probe_length, &plan
    );
    if (status == LT48_OK) {
        items[5].availability = LT48_OPERATION_EXECUTABLE;
        items[5].support_level = "engine_hardware_verified";
        items[5].reason =
            "mode-5 acquisition uses the validated read owner and compares the immutable image on the host";
    }
    lt48_protocol_plan_close(plan);
    if (audit->complete_group_equivalence_verified) {
        for (size_t i = 0u; i < 6u; ++i) {
            if (items[i].availability == LT48_OPERATION_EXECUTABLE) {
                items[i].support_level = "source_group_equivalence_verified";
                items[i].reason =
                    "source-bound 2764 group: complete procedure and common-range transactions match the validated 27128 engine; inherited qualification, no new 2764 hardware test";
            }
        }
    }
    if (audit->complete_source_audit_verified) {
        for (size_t i=0;i<6;++i) if (items[i].availability==LT48_OPERATION_EXECUTABLE) {
            items[i].support_level=(entry.device_class==2001&&entry.device_type==0x81)?"holtek040one_source_audited":(entry.device_class==2001&&entry.device_type==0x77)?"hitachipulse2_source_audited":tmp1962one_member(&entry)?"tmp1962one_source_audited":lt48_lpc16_member_internal(entry.device_class,entry.device_type)?"lpc16_source_audited":nand2_binding(&entry)?"nand2_source_audited":nand15_binding(&entry)?"nand15_source_audited":emtc18_binding(&entry)?"emtc18_source_audited":lt48_nxp18_member_internal(entry.device_class,entry.device_type)?"nxp18_source_audited":lt48_sharp13_member_internal(entry.device_class,entry.device_type)?"sharp13_source_audited":lt48_parallel20_member_internal(entry.device_class,entry.device_type)?"parallel20_source_audited":(entry.device_class==2001u&&entry.device_type>=0x3c&&entry.device_type<=0x40)?"novatek_umc6_source_audited":(entry.device_class==2001u&&(entry.device_type==0x25||entry.device_type==0x3a||entry.device_type==0x3b))?"eprom3_source_audited":audit->capacity_bytes>=131072u?"large_eprom_source_audited":"byte_eprom_source_audited";
            items[i].reason="Exact original EPROM resources and complete native operation contract audited; hardware testing pending";
        }
    }
    if(audit->complete_source_audit_verified &&
       ((entry.device_class==2001u && (entry.device_type==75u||entry.device_type==0x77u||(entry.device_type>=0x3c&&entry.device_type<=0x40))) ||
        (entry.device_class==2006u && entry.device_type==15u))) {
        lt48_gaaf_serial_resources *tms=NULL;
        if(lt48_gaaf_serial_open(installation,device_index,&tms)==LT48_OK) {
            items[LT48_OPERATION_SECURE-1].availability=LT48_OPERATION_EXECUTABLE;
            for(unsigned i=0;i<8;i++)if(i!=LT48_OPERATION_ERASE-1 && items[i].availability==LT48_OPERATION_EXECUTABLE) {
                items[i].support_level=entry.device_type==0x77?"hitachipulse2_source_audited":entry.device_type==75u||entry.device_class==2006?"tms320e_source_audited":"novatek_umc6_source_audited";
                items[i].reason="Exact selected EPROM array and source protection procedures; no electrical erase; source audit, hardware pending";
            }
        }
        lt48_gaaf_serial_close(tms);
    }
    if(audit->device_class==2006u && audit->device_type==0x0eu && audit->complete_source_audit_verified) {
        for(unsigned i=0;i<6;i++)if(i==0||i==4||i==5){
            items[i].support_level="mask_rom_source_audited";
            items[i].reason="Exact source mask-ROM read/verify/compare; source programming parameters disabled; no mutating operation";
        }else {items[i].availability=LT48_OPERATION_UNAVAILABLE;items[i].reason="Mask ROM: programming and blank checking are not applicable";}
    }
    if(lt48_m37_resources_is_eep21(resources)&&audit->complete_source_audit_verified){
        for(unsigned i=0;i<7;i++){items[i].availability=LT48_OPERATION_EXECUTABLE;
            items[i].support_level=(entry.device_class==2001u&&entry.device_type==0x28u&&!strcmp(entry.vendor,"ST"))?"m26c201one_source_audited":(entry.device_class==2001u&&(entry.device_type==0x26u||entry.device_type==0x27u)&&!strcmp(entry.vendor,"Macronix"))?"macronixmtp2_source_audited":tmp1962one_member(&entry)?"tmp1962one_source_audited":lt48_lpc16_member_internal(entry.device_class,entry.device_type)?"lpc16_source_audited":nand2_binding(&entry)?"nand2_source_audited":nand15_binding(&entry)?"nand15_source_audited":emtc18_binding(&entry)?"emtc18_source_audited":lt48_nxp18_member_internal(entry.device_class,entry.device_type)?"nxp18_source_audited":lt48_sharp13_member_internal(entry.device_class,entry.device_type)?"sharp13_source_audited":lt48_parallel20_member_internal(entry.device_class,entry.device_type)?"parallel20_source_audited":(entry.device_class==2001u&&entry.device_type>=0x3c&&entry.device_type<=0x40)?"novatek_umc6_source_audited":(entry.device_class==2001u&&(entry.device_type==0x25||entry.device_type==0x3a||entry.device_type==0x3b))?"eprom3_source_audited":at29_binding(&entry)?"at29_13_source_audited":green14_binding(&entry)?"green14_source_audited":"electrical_eprom_source_audited";
            items[i].reason=(entry.device_type==0x26u||entry.device_type==0x27u)?"Macronix MTP original zero-fill/erase-verify/final-pulse workflow and independent post-power-cycle blank verification; hardware pending":"Exact SST-compatible electrical EPROM workflows, source erase and post-power-cycle blank verification; hardware pending";}
    }
    if(audit->device_class==2006u && audit->device_type==5u && audit->complete_source_audit_verified) {
        plan=NULL;status=lt48_m37_resources_build_electrical_erase_plan(resources,0,65536,&plan);
        if(status==LT48_OK)for(size_t i=0;i<7;i++) {
            items[i].availability=LT48_OPERATION_EXECUTABLE;
            items[i].support_level="w27_source_audited";
            items[i].reason="Exact W27C512/W27E512 source electrical erase and Rapid program/verify, full-array blank check after power removal; hardware testing pending";
        }
        lt48_protocol_plan_close(plan);
    }
    lt48_m37_resources_close(resources);
    return LT48_OK;
}

const char *lt48_operation_kind_name(lt48_operation_kind operation) {
    static const char *const names[LT48_OPERATION_KIND_COUNT] = {
        "read", "blank-check", "program", "over-program", "verify",
        "compare", "erase", "secure", "memory-protect", "configuration",
        "functional-test", "auto-id", "mass-production", "jtag-isp"
    };
    unsigned value = (unsigned)operation;
    return value >= 1u && value <= LT48_OPERATION_KIND_COUNT ?
        names[value - 1u] : NULL;
}

lt48_status lt48_operation_kind_parse(
    const char *name, lt48_operation_kind *out_operation
) {
    if (name == NULL || out_operation == NULL) return LT48_INVALID_ARGUMENT;
    for (unsigned value = 1u; value <= LT48_OPERATION_KIND_COUNT; ++value) {
        if (strcmp(name, lt48_operation_kind_name((lt48_operation_kind)value)) == 0) {
            *out_operation = (lt48_operation_kind)value;
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_installation_operation_support_one(
    const lt48_installation *installation, size_t device_index,
    lt48_operation_kind operation, lt48_operation_support *out_support
) {
    lt48_operation_support items[LT48_OPERATION_KIND_COUNT];
    size_t count = 0u;
    lt48_status status;
    if (out_support == NULL || lt48_operation_kind_name(operation) == NULL)
        return LT48_INVALID_ARGUMENT;
    status = lt48_installation_operation_support(
        installation, device_index, items, LT48_OPERATION_KIND_COUNT, &count
    );
    if (status != LT48_OK) return status;
    for (size_t index = 0u; index < count; ++index) {
        if (items[index].operation == operation) {
            *out_support = items[index];
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

static unsigned complete_operation_mask_for_class(uint16_t device_class) {
    switch (device_class) {
    case 2015u:
    case 2030u:
    case 2031u:
    case 2044u:
    case 2045u:
    case 2047u:
    case 2048u:
    case 2025u: return 0xffu;
    case 2005u: return 0xffu; /* Exact X28C64 owner only. */
    case 2032u: /* Exact Intel byte NOR, seven source operations. */
    case 2033u: /* Exact AT17F word-stream configuration flash. */
        return 0x7fu;
    case 2000u: /* Digest-bound Philips PROM and MCM68766 EPROM definitions. */
    case 2004u:
    case 2003u: /* Only digest-bound standard word EPROM definitions. */
    case 2034u: /* Only digest-bound Hitachi XP EPROM definitions. */
    case 2012u: /* Only source-approved small EPROM entries are admitted. */
    case 2001u: /* parallel EPROM; physical UV erase is external */
    case 2006u: /* Exact admitted Rapid EPROM definitions only. */
    case 2009u: /* Exact admitted CY27C256 definition only. */
        return (1u << (LT48_OPERATION_READ - 1u)) |
               (1u << (LT48_OPERATION_BLANK_CHECK - 1u)) |
               (1u << (LT48_OPERATION_PROGRAM - 1u)) |
               (1u << (LT48_OPERATION_OVER_PROGRAM - 1u)) |
               (1u << (LT48_OPERATION_VERIFY - 1u)) |
               (1u << (LT48_OPERATION_COMPARE - 1u));
    case 2013u:
    case 2018u:
        return (1u << (LT48_OPERATION_READ - 1u)) |
               (1u << (LT48_OPERATION_BLANK_CHECK - 1u)) |
               (1u << (LT48_OPERATION_PROGRAM - 1u)) |
               (1u << (LT48_OPERATION_OVER_PROGRAM - 1u)) |
               (1u << (LT48_OPERATION_VERIFY - 1u)) |
               (1u << (LT48_OPERATION_COMPARE - 1u)) |
               (1u << (LT48_OPERATION_SECURE - 1u));
    case 2027u: /* POF EPM */
    case 2024u: /* compact EJed CPLD */
    case 2020u: /* GAL */
    case 2023u: /* PALCE */
        return (1u << (LT48_OPERATION_READ - 1u)) |
               (1u << (LT48_OPERATION_BLANK_CHECK - 1u)) |
               (1u << (LT48_OPERATION_PROGRAM - 1u)) |
               (1u << (LT48_OPERATION_VERIFY - 1u)) |
               (1u << (LT48_OPERATION_ERASE - 1u)) |
               (1u << (LT48_OPERATION_SECURE - 1u));
    case 2019u: /* Compact PEEL/Atmel/Philips PLD; optional source Erase. */
    case 2022u: /* Source-selected compact PLD / clock operations. */
    case 2021u: /* one-time programmable PAL */
        return (1u << (LT48_OPERATION_READ - 1u)) |
               (1u << (LT48_OPERATION_BLANK_CHECK - 1u)) |
               (1u << (LT48_OPERATION_PROGRAM - 1u)) |
               (1u << (LT48_OPERATION_VERIFY - 1u)) |
               (1u << (LT48_OPERATION_SECURE - 1u));
    default:
        return 0u;
    }
}

const char *lt48_complete_operation_profile(uint16_t device_class) {
    switch (device_class) {
    case 2015u: return "pic-eight-operation";
    case 2048u: return "pic18-eight-operation";
    case 2011u: return "battery-sram-seven-operation";
    case 2042u: return "page-wide-seven-operation";
    case 2043u: return "serial-page-seven-operation";
    case 2005u: return "page-eeprom-eight-operation";
    case 2001u: return "eprom-six-operation";
    case 2033u: return "at17f-seven-operation";
    case 2031u: return "xp-flash-eight-operation";
    case 2032u: return "intel-byte-seven-operation";
    case 2045u: return "addtab-flash-eight-operation";
    case 2047u: return "addtab-otp-eight-operation";
    case 2044u: return "addtab-spi-eight-operation";
    case 2006u: return "rapid-eprom-six-operation";
    case 2009u: return "cypress-eprom-six-operation";
    case 2000u: return "bipolar-prom-six-operation";
    case 2004u:
    case 2003u: return "word-eprom-six-operation";
    case 2012u: return "compact-eprom-six-operation";
    case 2034u: return "hitachi-xp-eprom-six-operation";
    case 2013u:
    case 2018u: return "mcu51-code-encryption-security";
    case 2027u: return "epm-six-operation";
    case 2025u: return "xc95-eight-operation";
    case 2024u: return "ejed-six-operation";
    case 2020u: return "gal-six-operation";
    case 2022u:
    case 2019u: return "compact-pld-source-operation";
    case 2021u: return "pal-five-operation";
    case 2023u: return "palce-six-operation";
    default: return "unclassified";
    }
}

const char *lt48_installation_complete_operation_profile(const lt48_installation *x,size_t device) {
    lt48_catalogue_entry entry;if(lt48_installation_get_device(x,device,&entry)!=LT48_OK)return "unclassified";
    if((entry.device_class==2038u||entry.device_class==2044u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_mf74_admitted_internal(entry.device_class,entry.device_type)&&!lt48_mf74_open(x,device,&r)){lt48_s8253_close(r);return "mf74-nine-operation";}}
    if((entry.device_class==2038u||entry.device_class==2044u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_mw27_admitted_internal(entry.device_class,entry.device_type)&&!lt48_mw27_open(x,device,&r)){lt48_s8253_close(r);return "mw27-nine-operation";}}
    if(entry.device_class==2048u&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sy80_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sy80_open(x,device,&r)){lt48_s8253_close(r);return "sy80-nine-operation";}}
    if(entry.device_class==2048u){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sy76_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sy76_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if(entry.device_class==2038u){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_mw36_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mw36_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sm30_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sm30_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sx30_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sx30_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_mg15_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mg15_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sn9_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sn9_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_ob16_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_ob16_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2035u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_so30_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_so30_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_wt10_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_wt10_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_wt8_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_wt8_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_hy14_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_hy14_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_mw6_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mw6_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_my7_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_my7_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_my4_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_my4_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sc2_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sc2_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sc4g_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sc4g_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sc4a_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sc4a_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_tp4_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_tp4_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_tp6_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_tp6_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_mw8_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_mw8_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_gc8_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_gc8_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2039u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_ab11_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_ab11_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_su16_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_su16_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sm3_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sm3_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if((entry.device_class==2038u||entry.device_class==2048u)){lt48_device_spec z;if(!lt48_installation_resolve_device(x,device,&z)&&lt48_sm6_capacity_internal(entry.device_class,entry.device_type,z.family_code,z.pinmap)&&!lt48_sm6_admitted_internal(entry.device_class,entry.device_type))return "unclassified";}
    if(entry.device_class==2048u&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sy76_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sy76_open(x,device,&r)){lt48_s8253_close(r);return "sy76-nine-operation";}}
    if(entry.device_class==2038u&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_mw36_admitted_internal(entry.device_class,entry.device_type)&&!lt48_mw36_open(x,device,&r)){lt48_s8253_close(r);return "mw36-nine-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sm30_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sm30_open(x,device,&r)){lt48_s8253_close(r);return "sm30-nine-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sx30_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sx30_open(x,device,&r)){lt48_s8253_close(r);return "sx30-nine-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_mg15_admitted_internal(entry.device_class,entry.device_type)&&!lt48_mg15_open(x,device,&r)){lt48_s8253_close(r);return "mg15-nine-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sn9_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sn9_open(x,device,&r)){int array=lt48_snas4_array_internal(r->definition);lt48_s8253_close(r);return array?"snas4-seven-operation":"sn9-nine-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_ob16_admitted_internal(entry.device_class,entry.device_type)&&!lt48_ob16_open(x,device,&r)){lt48_s8253_close(r);return "ob16-eight-operation";}}
    if(lt48_tms87_admitted_internal(entry.device_class,entry.device_type)){lt48_tms87_resources*r=NULL;if(!lt48_tms87_open(x,device,&r)){lt48_tms87_close(r);return "tms87two-six-operation";}return "";}
    if(lt48_tms370_admitted_internal(entry.device_class,entry.device_type)){lt48_tms370_resources*r=NULL;if(!lt48_tms370_open(x,device,&r)){lt48_tms370_close(r);return "tms370seven-six-operation";}return "";}
    if(lt48_st7_admitted_internal(entry.device_class,entry.device_type)){lt48_st7_resources*r=NULL;if(!lt48_st7_open(x,device,&r)){lt48_st7_close(r);return "st7twelve-nine-operation";}return "";}
    if(lt48_r8c8_admitted_internal(entry.device_class,entry.device_type)){lt48_s8253_resources*r=NULL;if(!lt48_r8c8_open(x,device,&r)){lt48_s8253_close(r);return "r8c8-nine-operation";}return "";}
    if(ite2_member(&entry)){lt48_s8253_resources*r=NULL;if(!lt48_ite2_open(x,device,&r)){lt48_s8253_close(r);return "ite2-seven-operation";}return "";}
    if((entry.device_class==2035u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_so30_admitted_internal(entry.device_class,entry.device_type)&&!lt48_so30_open(x,device,&r)){lt48_s8253_close(r);return "so30-eight-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_wt10_admitted_internal(entry.device_class,entry.device_type)&&!lt48_wt10_open(x,device,&r)){lt48_s8253_close(r);return "wt10-seven-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_wt8_admitted_internal(entry.device_class,entry.device_type)&&!lt48_wt8_open(x,device,&r)){lt48_s8253_close(r);return "wt8-seven-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_hy14_admitted_internal(entry.device_class,entry.device_type)&&!lt48_hy14_open(x,device,&r)){lt48_s8253_close(r);return "hy14-eight-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_mw6_admitted_internal(entry.device_class,entry.device_type)&&!lt48_mw6_open(x,device,&r)){lt48_s8253_close(r);return "mw6-nine-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_my7_admitted_internal(entry.device_class,entry.device_type)&&!lt48_my7_open(x,device,&r)){lt48_s8253_close(r);return "my7-seven-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_my4_admitted_internal(entry.device_class,entry.device_type)&&!lt48_my4_open(x,device,&r)){unsigned fields=r->fields;lt48_s8253_close(r);return fields?"my4-nine-operation":"my4-seven-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sc2_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sc2_open(x,device,&r)){unsigned fields=r->fields;lt48_s8253_close(r);return fields?"sc2-nine-operation":"sc2-seven-operation";}}
    if((entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sc4g_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sc4g_open(x,device,&r)){unsigned fields=r->fields;lt48_s8253_close(r);return fields?"sc4g-nine-operation":"sc4g-seven-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sc4a_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sc4a_open(x,device,&r)){unsigned fields=r->fields;lt48_s8253_close(r);return fields?"sc4a-nine-operation":"sc4a-seven-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_tp4_admitted_internal(entry.device_class,entry.device_type)&&!lt48_tp4_open(x,device,&r)){unsigned fields=r->fields;lt48_s8253_close(r);return fields?"tp4-nine-operation":"tp4-seven-operation";}}
    if((entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_tp6_admitted_internal(entry.device_class,entry.device_type)&&!lt48_tp6_open(x,device,&r)){unsigned fields=r->fields;lt48_s8253_close(r);return fields?"tp6-nine-operation":"tp6-seven-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_mw8_admitted_internal(entry.device_class,entry.device_type)&&!lt48_mw8_open(x,device,&r)){lt48_s8253_close(r);return "mw8-nine-operation";}}
    if((entry.device_class==2038u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_gc8_admitted_internal(entry.device_class,entry.device_type)&&!lt48_gc8_open(x,device,&r)){lt48_s8253_close(r);return "gc8-seven-operation";}}
    if((entry.device_class==2039u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_ab11_admitted_internal(entry.device_class,entry.device_type)&&!lt48_ab11_open(x,device,&r)){lt48_s8253_close(r);return "ab11-nine-operation";}}
    if((entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_su16_admitted_internal(entry.device_class,entry.device_type)&&!lt48_su16_open(x,device,&r)){lt48_s8253_close(r);return "su16-eight-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sm3_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sm3_open(x,device,&r)){lt48_s8253_close(r);return "sm3-eight-operation";}}
    if((entry.device_class==2038u||entry.device_class==2048u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_sm6_admitted_internal(entry.device_class,entry.device_type)&&!lt48_sm6_open(x,device,&r)){lt48_s8253_close(r);return "sm6-eight-operation";}}
    if(entry.device_class==2009u&&entry.device_type>=0x64u&&entry.device_type<=0x68u){lt48_gaaf_serial_resources*r=NULL;if(!lt48_gaaf_serial_open(x,device,&r)){int ok=lt48_gaaf_serial_qualified(r);lt48_gaaf_serial_close(r);return ok?"gaaf-serial-six-operation":"unclassified";}}
    if(entry.device_class==2003u){lt48_gaaf_serial_resources*r=NULL;if(!lt48_utron4_open_internal(x,device,&r)){int ok=lt48_gaaf_serial_qualified(r);lt48_gaaf_serial_close(r);return ok?"utron4-maskrom-four-operation":"unclassified";}}
    if(entry.device_class==2003u){lt48_gaaf_serial_resources*r=NULL;if(!lt48_word14_open_internal(x,device,&r)){int ok=lt48_gaaf_serial_qualified(r),eprom=entry.device_type==0x39;lt48_gaaf_serial_close(r);return ok?(eprom?"word14-maskrom-four-operation":"legacy-nor-seven-operation"):"unclassified";}}
    if((entry.device_class==2038u||entry.device_class==2033u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_at91nine_admitted_internal()&&!lt48_at91nine_open(x,device,&r)){int complete=r->a91_width==2||(r->configuration_maximum[1]<=255&&r->capacity<=65536);lt48_s8253_close(r);if(complete)return "at91nine-nine-operation";}}
    if(entry.device_class==2018u&&entry.device_type>=0x112&&entry.device_type<=0x115&&!strcmp(entry.vendor,"Atmel")&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_t89six_admitted_internal(entry.device_type)&&!lt48_t89six_open(x,device,&r)){lt48_s8253_close(r);return "t89six-nine-operation";}return "";}
    if(lt48_ramtron21_member_internal(&entry)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(!lt48_ramtron6_open(x,device,&r)){lt48_s8253_close(r);return "ramtron6-nine-operation";}return "";}
    if(entry.device_class==2038u&&!strcmp(entry.vendor,"RAMTRON")&&(entry.device_type==172u||entry.device_type==173u||entry.device_type==176u||entry.device_type==177u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_ramtron6_admitted_internal()&&!lt48_ramtron6_open(x,device,&r)){lt48_s8253_close(r);return "ramtron6-nine-operation";}return "";}
    if(entry.device_class==2041u&&entry.device_type>=15u&&entry.device_type<=17u&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(!lt48_white3_open(x,device,&r)){lt48_s8253_close(r);return "white3-seven-operation";}return "";}
    if(entry.device_class==2041u&&(entry.device_type==18u||entry.device_type==19u)&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_eep2041_3_admitted_internal()&&!lt48_eep2041_3_open(x,device,&r)){lt48_s8253_close(r);return "eep2041-three-seven-operation";}return "";}
    if(entry.device_class==2044u&&entry.device_type==230u&&!strcmp(lt48_installation_source_profile(x),"Dataman 13.10")){lt48_s8253_resources*r=NULL;if(lt48_at28c64b1_admitted_internal()&&!lt48_at28c64b1_open(x,device,&r)){lt48_s8253_close(r);return "at28c64b1-seven-operation";}return "";}
    if(entry.device_class==2048u&&lt48_stm8s4_member_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_stm8s4_admitted_internal(entry.device_type)&&lt48_stm8s4_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "stm8s4-nine-operation";}return "";}
    if(entry.device_class==2048u&&lt48_cc254x5_member_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_cc254x5_admitted_internal(entry.device_type)&&lt48_cc254x5_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "cc254x5-nine-operation";}return "";}
    if(entry.device_class==2048u&&entry.device_type==205u){lt48_s8253_resources*r=NULL;if(lt48_sst58rc2_admitted_internal(entry.device_type)&&lt48_sst58rc2_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "sstmx16-nine-operation";}return "";}
    if(entry.device_class==2048u&&lt48_novatek25_member_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_novatek25_admitted_internal(entry.device_type)&&lt48_novatek25_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "novatek25-seven-operation";}return "";}
    if(entry.device_class==2048u&&(entry.device_type==0x1c8u||(entry.device_type>=0x1d1u&&entry.device_type<=0x1d5u))){lt48_s8253_resources*r=NULL;if(lt48_nuvoton8_admitted_internal(entry.device_type)&&lt48_nuvoton8_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "nuvoton8-nine-operation";}return "";}
    if(entry.device_class==2048u&&entry.device_type>=0x26du&&entry.device_type<=0x271u){lt48_s8253_resources*r=NULL;if(lt48_xmega7_admitted_internal(entry.device_type)&&lt48_xmega7_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "xmega7-nine-operation";}return "";}
    if(entry.device_class==2038u&&lt48_psoc29_admitted_internal(entry.device_type)){lt48_s8253_resources*r=NULL;if(lt48_psoc29_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "psoc-eight-operation";}}
    if(entry.device_class==2048u){lt48_pic24_resources *pic=NULL;if(lt48_pic24_open(x,device,&pic)==LT48_OK){lt48_pic24_close(pic);return "pic24-nine-operation";}}
    if(entry.device_class==2015u){lt48_pic_resources *pic=NULL;if(lt48_pic_open(x,device,&pic)==LT48_OK){uint32_t mask=lt48_pic_operation_mask_internal(pic);lt48_pic_close(pic);if(mask==0xbfu)return "pic-otp-seven-operation";}}
    if((entry.device_class==2001u && (entry.device_type==75u||entry.device_type==0x77u||(entry.device_type>=0x3c&&entry.device_type<=0x40)))||(entry.device_class==2006u && entry.device_type==15u)){
        lt48_gaaf_serial_resources *r=NULL;
        if(lt48_gaaf_serial_open(x,device,&r)==LT48_OK){lt48_gaaf_serial_close(r);return entry.device_class==2001&&entry.device_type==0x77?"hitachi-pulse-seven-operation":entry.device_class==2001&&entry.device_type!=75u?"novatek-umc-seven-operation":"tms320e-seven-operation";}
    }
    if(entry.device_class==2026u||entry.device_class==2028u){
        lt48_pof_device *pof=NULL;
        if(lt48_pof_device_open(x,device,&pof)==LT48_OK){lt48_pof_device_close(pof);return "pof-eight-operation";}
    }
    if(lt48_tpi15_binding_internal(&entry)&&lt48_tpi15_admitted_internal()){lt48_xp_flash_resources*r=NULL;if(!lt48_xp_flash_open(x,device,&r)){lt48_xp_flash_close(r);return "tpi-seven-operation";}}
    if(lt48_xp13_binding_internal(&entry)&&lt48_xp13_admitted_internal()){lt48_xp_flash_resources*r=NULL;if(lt48_xp_flash_open(x,device,&r)==LT48_OK){lt48_xp_flash_close(r);return entry.device_class==2035?"xp-otp-six-operation":"xp-shared-word-seven-operation";}}
    if(lt48_lpc16_member_internal(entry.device_class,entry.device_type)&&lt48_lpc16_admitted_internal(entry.device_type)){lt48_xp_flash_resources*r=NULL;if(!lt48_xp_flash_open(x,device,&r)){lt48_xp_flash_close(r);return "xp-shared-word-seven-operation";}}
    if(lt48_nxp18_member_internal(entry.device_class,entry.device_type)&&lt48_nxp18_admitted_internal(entry.device_type)){lt48_xp_flash_resources*r=NULL;if(!lt48_xp_flash_open(x,device,&r)){lt48_xp_flash_close(r);return "xp-shared-word-seven-operation";}}
    if(lt48_sharp13_member_internal(entry.device_class,entry.device_type)&&lt48_sharp13_admitted_internal(entry.device_type)){lt48_xp_flash_resources*r=NULL;if(!lt48_xp_flash_open(x,device,&r)){lt48_xp_flash_close(r);return "xp-shared-word-seven-operation";}}
    if(lt48_parallel20_member_internal(entry.device_class,entry.device_type)&&lt48_parallel20_admitted_internal(entry.device_type)){lt48_xp_flash_resources*r=NULL;if(!lt48_xp_flash_open(x,device,&r)){lt48_xp_flash_close(r);return "xp-shared-word-seven-operation";}}
    if(entry.device_class==2031u && (lv800three_member(&entry) || (entry.device_type==0xb4 || entry.device_type==0x5b) || ((entry.device_type>=0x107 && entry.device_type<=0x10a) || entry.device_type==0x136 || (entry.device_type>=0x10c && entry.device_type<=0x10e) || entry.device_type==0xfd || entry.device_type==0xff || entry.device_type==0x100)||xp_shared_word_batch(entry.device_type)||xp_atmel_sanyo15_batch(entry.device_type)||(entry.device_type>=0xdd && entry.device_type<=0xe8))){lt48_xp_flash_resources *r=NULL;if(lt48_xp_flash_open(x,device,&r)==LT48_OK){lt48_xp_flash_close(r);return "xp-shared-word-seven-operation";}}
    if(entry.device_class==2030u){lt48_xp_flash_resources *r=NULL;if(lt48_xp_flash_open(x,device,&r)==LT48_OK){unsigned kind=lt48_xp_flash_secure_kind(r);lt48_xp_flash_close(r);return kind?"xp-byte-eight-operation":"xp-byte-seven-operation";}}
    if((page_byte21_batch(entry.device_class,entry.device_type)||(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type)))))){lt48_xp_flash_resources *r=NULL;if(lt48_xp_flash_open(x,device,&r)==LT48_OK){unsigned kind=lt48_xp_flash_secure_kind(r);lt48_xp_flash_close(r);return kind?"page-byte-eight-operation":"page-byte-seven-operation";}}
    if(entry.device_class==2042u){lt48_xp_flash_resources *r=NULL;if(lt48_xp_flash_open(x,device,&r)==LT48_OK){unsigned kind=lt48_xp_flash_secure_kind(r);lt48_xp_flash_close(r);return kind?"page-wide-eight-operation":"page-wide-seven-operation";}}
    if(entry.device_class==2010u){lt48_lpcflash_resources*r=NULL;if(lt48_lpcflash_open(x,device,&r)==LT48_OK){lt48_lpcflash_close(r);return "lpc-flash-crc-eight-operation";}}
    if(entry.device_class==2039u&&entry.device_type==5u){lt48_s8253_resources*r=NULL;if(lt48_n76one_open(x,device,&r)==LT48_OK){lt48_s8253_close(r);return "n79w79_21-nine-operation";}}
    #include "generated/config_fields_profiles.inc"

    if(entry.device_class==2048u){lt48_s8253_resources*r=NULL;if(lt48_msp43076_open(x,device,&r)==LT48_OK){int admitted=lt48_msp43076_admitted_internal(entry.device_type);lt48_s8253_close(r);return admitted?"msp43076-nine-operation":"";}}
    if(entry.device_class==2038u){lt48_s8253_resources*r=NULL;if(lt48_sstmx16_open(x,device,&r)==LT48_OK){int admitted=lt48_sstmx16_admitted_internal(entry.device_type);lt48_s8253_close(r);return admitted?"sstmx16-nine-operation":"";}}
    if(entry.device_class==2048u){lt48_s8253_resources*r=NULL;if(lt48_n79w79_21_open(x,device,&r)==LT48_OK){int admitted=lt48_n79w79_21_admitted_internal(entry.device_type);lt48_s8253_close(r);return admitted?"n79w79_21-nine-operation":"";}}
    if(entry.device_class==2048u){lt48_s8253_resources*r=NULL;if(lt48_nuvoton30_open(x,device,&r)==LT48_OK){int admitted=lt48_nuvoton30_admitted_internal(entry.device_type);lt48_s8253_close(r);return admitted?"nuvoton30-nine-operation":"";}}
    if(entry.device_class==2048u){lt48_s8253_resources*r=NULL;if(lt48_numicro81_open(x,device,&r)==LT48_OK){int admitted=lt48_numicro81_admitted_internal(entry.device_type);lt48_s8253_close(r);return admitted?"numicro81-nine-operation":"";}}
    if(entry.device_class==2038u||entry.device_class==2048u){lt48_s8253_resources*r=NULL;if(lt48_n79w79_open(x,device,&r)==LT48_OK){int admitted=lt48_n79w79_admitted_internal(entry.device_class,entry.device_type);lt48_s8253_close(r);return admitted?"n79w79-nine-operation":"";}}
    if(entry.device_class==2018u){lt48_ic2_resources*r=NULL;if(lt48_ic2_open(x,device,&r)==LT48_OK){lt48_ic2_close(r);return "at89-ic2-nine-operation";}}
    if(entry.device_class==2048u){lt48_lp51_basic_resources*r=NULL;if(lt48_lp51_basic_open(x,device,&r)==LT48_OK){lt48_lp51_basic_close(r);return "at89-lp51-basic-nine-operation";}}
    if(entry.device_class==2048u){lt48_s2051_resources*r=NULL;if(lt48_s2051_open(x,device,&r)==LT48_OK){lt48_s2051_close(r);return "at89-s2051-nine-operation";}}
    if(entry.device_class==2048u){lt48_re2_resources*r=NULL;if(lt48_re2_open(x,device,&r)==LT48_OK){lt48_re2_close(r);return "at89-re2-nine-operation";}}
    if(entry.device_class==2048u){lt48_s8253_resources*r=NULL;if(lt48_s8253_open(x,device,&r)==LT48_OK){const char*p=r->kind==4?(lt48_w78bank147_admitted_internal(r->definition)?"w78bank147-nine-operation":NULL):r->kind==3?(lt48_w78d24_admitted_internal(r->definition)?"w78d24-nine-operation":NULL):r->kind==2?(lt48_sst_admitted_internal(r->definition)?"sst89rd-nine-operation":NULL):r->kind?(lt48_p89v_admitted_internal(r->definition)?"p89v51-nine-operation":NULL):"at89-s8253-nine-operation";lt48_s8253_close(r);if(p)return p;}}
    if(entry.device_class==2048u){lt48_lp2052_resources*r=NULL;if(lt48_lp2052_open(x,device,&r)==LT48_OK){lt48_lp2052_close(r);return "at89-lp2052-nine-operation";}}
    if(entry.device_class==2048u){lt48_usb51_resources*r=NULL;if(lt48_usb51_open(x,device,&r)==LT48_OK){lt48_usb51_close(r);return "at89-usb51-nine-operation";}}
    if(entry.device_class==2048u){lt48_ed2_resources*r=NULL;if(lt48_ed2_open(x,device,&r)==LT48_OK){lt48_ed2_close(r);return "at89-ed2-nine-operation";}}
    if(entry.device_class==2048u){lt48_at89_lp51_resources*r=NULL;if(lt48_at89_lp51_open(x,device,&r)==LT48_OK){lt48_at89_lp51_close(r);return "at89-lp51-nine-operation";}}
    if(entry.device_class==2048u){lt48_at89_lp_large_resources*r=NULL;if(lt48_at89_lp_large_open(x,device,&r)==LT48_OK){lt48_at89_lp_large_close(r);return "at89-lp-large-nine-operation";}}
    if(entry.device_class==2045u && (addtab_remaining_admitted(entry.device_type)||addtab_exact17_batch(entry.device_type,entry.unknown6)||addtab_vid6_batch(entry.device_type)||addtab_winbond32_batch(entry.device_type)||addtab_mx26_batch(entry.device_type)||addtab_sst7_batch(entry.device_type)||addtab_sanyo5_batch(entry.device_type)||addtab_tail9_batch(entry.device_type)||addtab_tail24_batch(entry.device_type)||addtab_tail18_batch(entry.device_type)||addtab_dl46_batch(entry.device_type)||addtab_glp34_batch(entry.device_type)||addtab_gln20_batch(entry.device_type)||addtab_gln86_batch(entry.device_type)||addtab_glm22_batch(entry.device_type)||addtab_gl47_batch(entry.device_type)||addtab_gl57_batch(entry.device_type)||addtab_gl45_batch(entry.device_type)||addtab_ew18_batch(entry.device_type)||addtab_micron27_batch(entry.device_type)||addtab_ew54_batch(entry.device_type)||addtab_gl84_batch(entry.device_type)||addtab_gl67_batch(entry.device_type))) {
        lt48_addtab_flash_resources *r=NULL;
        if(lt48_addtab_flash_open(x,device,&r)==LT48_OK){unsigned secure=lt48_addtab_flash_secure_kind(r);lt48_addtab_flash_close(r);return secure?"addtab-flash-eight-operation":"addtab-gl-seven-operation";}
    }
    if(entry.device_class==2047u){lt48_addtab_flash_resources*r=NULL;if(lt48_addtab_flash_open(x,device,&r)==LT48_OK){unsigned secure=lt48_addtab_flash_secure_kind(r);lt48_addtab_flash_close(r);return secure?"addtab-otp-eight-operation":"addtab-otp-seven-operation";}}
    if(entry.device_class==2044u){lt48_addtab_flash_resources *r=NULL;if(lt48_addtab_flash_open(x,device,&r)==LT48_OK){lt48_addtab_flash_close(r);return "addtab-byte-seven-operation";}}
    if((entry.device_class==2044u || entry.device_class==2041u)){lt48_addtab_spi_resources*r=NULL;if(lt48_addtab_spi_open(x,device,&r)==LT48_OK){int secure=lt48_addtab_spi_validate(r,LT48_OPERATION_SECURE,0,lt48_addtab_spi_capacity(r),NULL,0)==LT48_OK;lt48_addtab_spi_close(r);return secure?"addtab-spi-eight-operation":"addtab-spi-seven-operation";}}
    if(entry.device_class==2033u && (mr7_member(&entry)||xp_sanyo4_batch(entry.device_type)||xp_seiko12_batch(entry.device_type)||xp_intel29_batch(entry.device_type)||intel_word100_batch(entry.device_type)||intel_shared_word118_batch(entry.device_type)||(entry.device_type>=0xbc && entry.device_type<=0xc1))) {
        lt48_xp_flash_resources *r=NULL;
        if(lt48_xp_flash_open(x,device,&r)==LT48_OK){unsigned secure=lt48_xp_flash_secure_kind(r);lt48_xp_flash_close(r);return secure?"intel-word-eight-operation":"intel-word-seven-operation";}
    }
    if(entry.device_class==2033u && entry.device_type<0xb0u)return "c3-main-array-seven-operation";
    if(entry.device_class==2011u || (entry.device_class==2034u && entry.device_type>=0x2a && entry.device_type<=0x2f)){
        lt48_small_eprom_resources*r=NULL;
        if(lt48_small_eprom_open(x,device,&r)==LT48_OK){int rtc=lt48_small_eprom_is_timekeeper(r);lt48_small_eprom_close(r);return entry.device_class==2011u&&entry.device_type<=6u&&!strcmp(entry.vendor,"Cypress")?"cypress-prom-six-operation":rtc?"timekeeper-eight-operation":"battery-sram-seven-operation";}
    }
    if(entry.device_class==2017u){lt48_xc7000_resources*r=NULL;if(lt48_xc7000_open(x,device,&r)==LT48_OK){lt48_xc7000_close(r);return "xc7000-seven-operation";}}
    if(entry.device_class==2018u){lt48_mcu51_resources*r=NULL;if(lt48_mcu51_open(x,device,&r)==LT48_OK){if(lt48_mcu51_cl38(r)){int admitted=lt48_mcu51_cl38_admitted(r);unsigned m=lt48_mcu51_cl38_family(r),mask=lt48_mcu51_cl38_mask(r);lt48_mcu51_close(r);return !admitted?"":m>=128&&m<=135?"dallas-flash-nine-operation":mask==0x7f?"cl38-memory-seven-operation":mask==0x2bf?"cl38-typed-eight-operation":"cl38-typed-nine-operation";}lt48_mcu51_close(r);}}
    if(entry.device_class==2018u){lt48_mcu51_resources*r=NULL;if(lt48_mcu51_open(x,device,&r)==LT48_OK){unsigned p=lt48_mcu51_philips_options(r),otp=lt48_mcu51_is_otp20(r);int electrical=lt48_mcu51_is_mcu70(r)&&lt48_mcu51_validate(r,LT48_OPERATION_ERASE,0,lt48_mcu51_capacity(r),NULL,0)==LT48_OK;lt48_mcu51_close(r);if(electrical)return (p==8||p==11)?"rd2-flash-nine-operation":p==7?"dallas-flash-nine-operation":"flash51-eight-operation";if(p>=2)return "philips39-eight-operation";if(p||otp)return "philips39-seven-operation";}}
    if(entry.device_class==2013u){lt48_mcu51_resources*r=NULL;if(lt48_mcu51_open(x,device,&r)==LT48_OK){unsigned lm20=lt48_mcu51_lm20_kind(r);if(lm20){unsigned mask=lt48_mcu51_lm20_admitted(r)?lt48_mcu51_lm20_mask(r):0;lt48_mcu51_close(r);return mask==0xff?"legacy-mcu20-flash-eight-operation":mask==0x27f?"legacy-mcu20-options-eight-operation":mask==0x2ff?"legacy-mcu20-flash-nine-operation":mask==0x2bf?"legacy-mcu20-otp-eight-operation":mask==0xbf?"legacy-mcu20-otp-seven-operation":mask==0x3f?"legacy-mcu20-six-operation":NULL;}unsigned l14=lt48_mcu51_legacy14_kind(r);if(l14){const char*p=!lt48_mcu51_legacy14_admitted(r)?NULL:(l14==8||l14==9)?"otp13five-six-operation":l14==10?"aduc812-seven-operation":l14==5?"legacy14-aduc-nine-operation":l14==4?"legacy14-nvram-eight-operation":l14>=6?"legacy14-flash-seven-operation":"legacy14-otp-seven-operation";lt48_mcu51_close(r);return p;}int at89=lt48_mcu51_is_at89(r),upi=lt48_mcu51_is_upi41ah(r),f41=lt48_mcu51_is_flash51_41(r);lt48_mcu51_close(r);if(f41)return f41>=16&&f41<=19?"flash51-nine-operation":"flash51-eight-operation";if(upi)return "upi41ah-eight-operation";if(at89)return at89==2?"at89-legacy-s-nine-operation":"at89-legacy-lv-eight-operation";}}
    if(entry.device_class==2013u && (entry.device_type>=0x8du && entry.device_type<=0x90u))return "mcs48-six-operation";
    if(entry.device_class==2006u && entry.device_type==0xeu)return "mask-rom-three-operation";
    if(lt48_eep21_definition_internal(entry.device_class,entry.device_type))return "electrical-eprom-seven-operation";
    if(entry.device_class==2006u && entry.device_type==5u)return "w27-seven-operation";
    if(tmp1962one_member(&entry)){lt48_xp_flash_resources*r=NULL;if(!lt48_xp_flash_open(x,device,&r)){lt48_xp_flash_close(r);return "tmp1962one-seven-operation";}return "";}
    if(sierra2_member(&entry)){lt48_xp_eprom_resources*r=NULL;if(!lt48_xp_eprom_open(x,device,&r)){lt48_xp_eprom_close(r);return "sierra2-seven-operation";}return "";}
    if(ptc010three_member(&entry)){lt48_xp_eprom_resources*r=NULL;if(!lt48_xp_eprom_open(x,device,&r)){lt48_xp_eprom_close(r);return "ptc010three-seven-operation";}return "";}
    if(gdacotp6_catalogue_member(&entry)){lt48_xp_eprom_resources*r=NULL;if(!lt48_xp_eprom_open(x,device,&r)){lt48_xp_eprom_close(r);return "gdacotp6-six-operation";}return "";}
    if(amic4_catalogue_member(&entry)){lt48_xp_eprom_resources*r=NULL;if(!lt48_xp_eprom_open(x,device,&r)){lt48_xp_eprom_close(r);return "amic4-otp-six-operation";}return "";}
    if(entry.device_class==2034u && (entry.device_type==3u || entry.device_type==4u)){lt48_xp_eprom_resources*r=NULL;if((entry.device_class!=2038u || entry.device_type<0x61 || entry.device_type>0x70 || lt48_w78legacy90_admitted_internal(entry.device_type)) && lt48_xp_eprom_open(x,device,&r)==LT48_OK){lt48_xp_eprom_close(r);return "ptc-xp-seven-operation";}}
    if(entry.device_class==2034u){lt48_gdac_serial_resources*r=NULL;if(lt48_gdac_serial_open(x,device,&r)==LT48_OK){int otp=lt48_gdac_serial_is_otp(r);lt48_gdac_serial_close(r);return otp?"serial-otp-five-operation":"gdac-serial-six-operation";}}
    if(sm2964two_member(&entry)){lt48_gaaf_serial_resources*r=NULL;if(!lt48_gaaf_serial_open(x,device,&r)){lt48_gaaf_serial_close(r);return "sm2964two-eight-operation";}}
    if(mx27pageone_member(&entry)){lt48_gaaf_serial_resources*r=NULL;if(!lt48_gaaf_serial_open(x,device,&r)){int ok=lt48_gaaf_serial_qualified(r);lt48_gaaf_serial_close(r);if(ok)return "mx27page-otp-six-operation";}return "";}
    if(entry.device_class==2009u||entry.device_class==2002u||entry.device_class==2007u||entry.device_class==2003u||entry.device_class==2008u||entry.device_class==2004u||entry.device_class==2005u||entry.device_class==2029u){lt48_gaaf_serial_resources*r=NULL;if(lt48_gaaf_serial_open(x,device,&r)==LT48_OK){int complete=lt48_gaaf_serial_qualified(r)&&lt48_gaaf_serial_validate(r,LT48_OPERATION_ERASE,0,lt48_gaaf_serial_capacity(r),NULL,0,LT48_VERIFY_MARGIN_ALL,0)==LT48_OK;unsigned cfg=lt48_gaaf_serial_configuration_bits(r);lt48_gaaf_serial_close(r);if(complete)return (st2009two_member(&entry)||entry.device_class==2007u||entry.device_class==2003u||entry.device_class==2008u||entry.device_class==2004u||entry.device_class==2005u||entry.device_class==2029u)?"legacy-nor-seven-operation":entry.device_class==2002u?"legacy28f-seven-operation":cfg==9?"w45d-eight-operation":cfg==8?"nexflash-eight-operation":"gaaf-serial-six-operation";}}
    if(entry.device_class==2014u&&!strcmp(entry.vendor,"SyncMos")){lt48_small_eprom_resources*r=NULL;if(!lt48_small_eprom_open(x,device,&r)){unsigned sg=lt48_small_eprom_is_sm2965four(r);lt48_small_eprom_close(r);if(sg)return "sm2965-nine-operation";}}
    if(entry.device_class==2014u){lt48_hc705_bootstrap_resources*r=NULL;if(lt48_hc705_bootstrap_open(x,device,&r)==LT48_OK){lt48_hc705_bootstrap_close(r);return "hc705-bootstrap-five-operation";}}
    if(entry.device_class==2014u){lt48_hc705_jp_resources*r=NULL;if(lt48_hc705_jp_open(x,device,&r)==LT48_OK){lt48_hc705_jp_close(r);return "hc705-jp-five-operation";}}
    if(entry.device_class==2014u){lt48_hc705_ca_resources*r=NULL;if(lt48_hc705_ca_open(x,device,&r)==LT48_OK){lt48_hc705_ca_close(r);return "hc705-ca-five-operation";}}
    if(entry.device_class==2014u){lt48_hc705_bd_resources*r=NULL;if(lt48_hc705_bd_open(x,device,&r)==LT48_OK){lt48_hc705_bd_close(r);return "hc705-bd-five-operation";}}
    if(entry.device_class==2014u){lt48_hc705_bx_resources*r=NULL;if(lt48_hc705_bx_open(x,device,&r)==LT48_OK){lt48_hc705_bx_close(r);return "hc705-bx-five-operation";}}
    if(entry.device_class==2014u){lt48_rs08_resources*r=NULL;if(lt48_rs08_open(x,device,&r)==LT48_OK){lt48_rs08_close(r);return "rs08-six-operation";}}
    if(entry.device_class==2014u){lt48_s08_resources*r=NULL;if(lt48_s08_open(x,device,&r)==LT48_OK){lt48_s08_close(r);return "s08-six-operation";}}
    if(entry.device_class==2014u){lt48_hc705_jjsrpl_resources*r=NULL;if(lt48_hc705_jjsrpl_open(x,device,&r)==LT48_OK){lt48_hc705_jjsrpl_close(r);return "hc705-jjsrpl-five-operation";}}
    if(entry.device_class==2012u){lt48_hc705_compact_resources*r=NULL;if(lt48_hc705_compact_open(x,device,&r)==LT48_OK){lt48_hc705_compact_close(r);return "hc705-compact-five-operation";}}
    if(entry.device_class==2013u){lt48_nmos68705_resources*r=NULL;if(lt48_nmos68705_open(x,device,&r)==LT48_OK){lt48_nmos68705_close(r);return "nmos68705-offline-five-operation";}}
    if(entry.device_class==2013u){lt48_hc711_resources*r=NULL;if(lt48_hc711_open(x,device,&r)==LT48_OK){lt48_hc711_close(r);return "hc711-eprom-five-operation";}}
    if(entry.device_class==2013u){lt48_hc11_resources*r=NULL;if(lt48_hc11_open(x,device,&r)==LT48_OK){int ext=lt48_hc11_is_extension(r);lt48_hc11_close(r);return ext?"hc11-eeprom-six-operation":"hc11-ael-seven-operation";}}
    if(entry.device_class==2014u){lt48_hc908_resources*r=NULL;if(lt48_hc908_open(x,device,&r)==LT48_OK){int ff=lt48_hc908_is_fqtqy(r),aa=lt48_hc908_is_abasaz(r),gg=lt48_hc908_is_gpgz(r),ap=lt48_hc908_is_apsr(r),gr=lt48_hc908_is_grkx(r),gx=lt48_hc908_is_gxjb(r),jk=lt48_hc908_is_jkjl(r);lt48_hc908_close(r);return ff?"hc908-fqtqy-six-operation":aa?"hc908-abasaz-six-operation":gg?"hc908-gpgz-six-operation":ap?"hc908-apsr-six-operation":gr?"hc908-grkx-six-operation":gx?"hc908-gxjb-six-operation":jk?"hc908-jkjl-six-operation":"hc908-qtqy-six-operation";}}
    if(entry.device_class==2009u){lt48_hitachi_resources*r=NULL;if(lt48_hitachi_open(x,device,&r)==LT48_OK){const char*p=lt48_hitachi_qualified(r)?(lt48_hitachi_is15_internal(r)?"hitachi15-five-operation":lt48_hitachi_is_fujitsu(r)?"fujitsu-mcu-six-operation":"hitachi-mcu-six-operation"):"unclassified";lt48_hitachi_close(r);return p;}}
    if(entry.device_class==2009u){lt48_legacy_dataflash_resources*r=NULL;if(lt48_legacy_dataflash_open(x,device,&r)==LT48_OK){lt48_legacy_dataflash_close(r);return "legacy-dataflash-six-operation";}}
    if(entry.device_class==2043u){lt48_serial_page_resources*r=NULL;if(lt48_serial_page_open(x,device,&r)==LT48_OK){int lp21=lt48_serial_page_is_lp21(r);unsigned kind=lt48_serial_page_i2c_config_kind(r);int secure=lt48_serial_page_validate(r,LT48_OPERATION_SECURE,0,lt48_serial_page_capacity(r),NULL,0)==LT48_OK;lt48_serial_page_close(r);if(lp21)return "at89-lp21-nine-operation";if(kind==4)return "at17n-eight-operation";if(kind)return kind==1?"page-i2c-eight-operation":"page-i2c-nine-operation";if(secure)return "spi-page-eight-operation";}}
    if(entry.device_class==2005u){lt48_page_eeprom_resources*r=NULL;if(lt48_page_eeprom_open(x,device,&r)==LT48_OK){int word=lt48_page_eeprom_is_word(r);int plain=lt48_page_eeprom_validate(r,LT48_OPERATION_SECURE,0,lt48_page_eeprom_capacity(r),NULL,0)==LT48_UNSUPPORTED;lt48_page_eeprom_close(r);if(word)return "at29word-eight-operation";if(plain)return "page-eeprom-seven-operation";}}
    if(entry.device_class==2010u && entry.device_type==0x91){lt48_stk10c68_resources*r=NULL;if(!lt48_stk10c68_open(x,device,&r)){lt48_stk10c68_close(r);return "stk11c68-seven-operation";}}
    if(entry.device_class==2010u && entry.device_type==0xfffe)return "stk10c68-seven-operation";
    if(entry.device_class==2010u||entry.device_class==2026u){lt48_config_memory_resources*r=NULL;if(lt48_config_memory_open(x,device,&r)==LT48_OK){const char*p=lt48_config_memory_profile_internal(r);lt48_config_memory_close(r);return p;}}
    if(entry.device_class==2010u){lt48_xc17_resources*r=NULL;if(lt48_xc17_open(x,device,&r)==LT48_OK){const char*p=lt48_xc17_profile_internal(r);lt48_xc17_close(r);return p;}}
    if(entry.device_class==2010u){lt48_avr_resources*r=NULL;if(lt48_avr_open(x,device,&r)==LT48_OK){lt48_avr_close(r);return "avr-eight-operation";}}
    if(entry.device_class==2000u){lt48_compact_spi_resources *r=NULL;if(lt48_compact_spi_open(x,device,&r)==LT48_OK){int secure=lt48_compact_spi_has_protection(r),idlock=lt48_compact_spi_has_idlock(r),raw=lt48_compact_spi_has_raw_status(r),incremental=lt48_compact_spi_has_incremental_registers(r);lt48_compact_spi_close(r);return incremental?"incremental-eight-operation":raw?"compact-flash-nine-operation":idlock?"idlock-nine-operation":secure?"compact-spi-eight-operation":"compact-spi-seven-operation";}}
    if(entry.device_class==2000u){lt48_compact_i2c_resources *r=NULL;if(lt48_compact_i2c_open(x,device,&r)==LT48_OK){int at17=lt48_compact_i2c_is_at17(r),secure=lt48_compact_i2c_has_protection(r),endurance=lt48_compact_i2c_has_endurance_config(r);unsigned spd=lt48_compact_i2c_spd_protection_bits(r);lt48_compact_i2c_close(r);if(at17)return "at17-compact-eight-operation";if(spd)return "spd-nine-operation";if(endurance)return "endurance-nine-operation";return secure?"compact-i2c-eight-operation":"compact-i2c-seven-operation";}}
    if(entry.device_class==2000u){lt48_microwire_resources *r=NULL;if(lt48_microwire_open(x,device,&r)==LT48_OK){int nvram=lt48_microwire_is_nvram(r);lt48_microwire_close(r);return nvram?"serial-nvram-six-operation":"microwire-seven-operation";}}
    if(entry.device_class==2010u&&!strcmp(entry.vendor,"Hyundai(Hynix)")){lt48_small_eprom_resources*r=NULL;if(!lt48_small_eprom_open(x,device,&r)){int member=lt48_small_eprom_is_hyundai18_internal(r);lt48_small_eprom_close(r);if(member)return "hyundai-xiprom-seven-operation";}}
    if(entry.device_class==2000u){lt48_small_eprom_resources*r=NULL;if(lt48_small_eprom_open(x,device,&r)==LT48_OK){int sx=lt48_small_eprom_is_scenix6(r),h13=lt48_small_eprom_is_hyundai13_internal(r),hy=lt48_small_eprom_is_hyundai25_internal(r),hc=lt48_small_eprom_is_eeprom(r),mcm=lt48_small_eprom_is_mcm68766(r),cy=lt48_small_eprom_is_cypress55(r);lt48_small_eprom_close(r);if(sx)return "scenix6-nine-operation";if(h13)return h13==166||h13==167?"hyundai13-six-operation":h13>=174?"hyundai13-options-seven-operation":"hyundai13-options-eight-operation";if(hy)return "hyundai-compact-otp-seven-operation";if(hc)return "atmel-hc-seven-operation";if(cy)return "cypress-usb-otp-seven-operation";if(mcm)return "mcm68766-six-operation";}}
    if(entry.device_class==2013u&&!strcmp(entry.vendor,"MYSON")){lt48_device_spec s;if(!lt48_installation_resolve_device(x,device,&s)&&mysonM2_binding(&s)){lt48_mcu51_resources*r=NULL;if(!lt48_mcu51_open(x,device,&r)){lt48_mcu51_close(r);return "mysonM2-seven-operation";}}}
    if(entry.device_class==2013u&&!strcmp(entry.vendor,"MYSON")){lt48_device_spec s;if(!lt48_installation_resolve_device(x,device,&s)&&myson44_binding(&s)){lt48_mcu51_resources*r=NULL;if(!lt48_mcu51_open(x,device,&r)){lt48_mcu51_close(r);return "myson44-seven-operation";}}}
    if(entry.device_class==2013u&&!strcmp(entry.vendor,"MYSON")){lt48_device_spec spec;if(!lt48_installation_resolve_device(x,device,&spec)&&myson2_binding(&spec)){lt48_mcu51_resources*r=NULL;if(!lt48_mcu51_open(x,device,&r)){lt48_mcu51_close(r);return "myson2-seven-operation";}}}
    if(entry.device_class==2013u&&!strcmp(entry.vendor,"ELAN")){lt48_small_eprom_resources*r=NULL;if(!lt48_small_eprom_open(x,device,&r)){unsigned el=lt48_small_eprom_is_elan5(r);lt48_small_eprom_close(r);if(el)return "elan5-options-seven-operation";}}
    if(entry.device_class==2012u){lt48_small_eprom_resources*r=NULL;if(!lt48_small_eprom_open(x,device,&r)){unsigned sy=lt48_small_eprom_is_syntek4(r);lt48_small_eprom_close(r);if(sy)return "syntek4-options-eight-operation";}}
    if(entry.device_class==2012u || (entry.device_class==2013u&&entry.device_type==0xc5)){lt48_small_eprom_resources *p=NULL;if(lt48_small_eprom_open(x,device,&p)==LT48_OK){unsigned parallel=lt48_small_eprom_is_hc711_parallel(p);lt48_small_eprom_close(p);if(parallel)return "hc711-parallel-five-operation";}}
    if(entry.device_class==2012u || (entry.device_class==2013u&&!strcmp(entry.vendor,"ST"))){lt48_small_eprom_resources *sr=NULL;if(lt48_small_eprom_open(x,device,&sr)==LT48_OK){int st=lt48_small_eprom_is_st62(sr),options=lt48_small_eprom_st62_option_mask(sr);lt48_small_eprom_close(sr);if(st)return options?"st62-options-eight-operation":"st62-otp-seven-operation";}}
    if(entry.device_class==2012u || (entry.device_class==2013u&&!strcmp(entry.vendor,"Zilog"))){lt48_small_eprom_resources *zr=NULL;if(lt48_small_eprom_open(x,device,&zr)==LT48_OK){int zi=lt48_small_eprom_is_zilog70(zr),word=lt48_small_eprom_is_zilog_word(zr);lt48_small_eprom_close(zr);if(word)return "zilog-word-otp-eight-operation";if(zi)return "zilog-otp-seven-operation";}}
    if(entry.device_class==2012u){lt48_small_eprom_resources*r=NULL;if(lt48_small_eprom_open(x,device,&r)==LT48_OK){int cop=lt48_small_eprom_is_cop48(r);lt48_small_eprom_close(r);if(cop)return "cop48-otp-seven-operation";}}
    if(entry.device_class==2012u){lt48_small_eprom_resources*r=NULL;if(lt48_small_eprom_open(x,device,&r)==LT48_OK){int family=lt48_small_eprom_is_compact40(r),ee=lt48_small_eprom_is_eeprom(r);lt48_small_eprom_close(r);if(family)return family>=27&&family<=30?"compact40-flash-eight-operation":family==16?"compact40-otp-seven-operation":ee?"compact-eeprom-seven-operation":"compact-eprom-six-operation";}}
    if(entry.device_class==2012u){lt48_small_eprom_resources *r=NULL;if(lt48_small_eprom_open(x,device,&r)==LT48_OK){int nv=lt48_small_eprom_is_nvram(r),ee=lt48_small_eprom_is_eeprom(r),at89=lt48_small_eprom_is_at89(r);lt48_small_eprom_close(r);if(at89)return "at89-compact-eight-operation";if(ee)return "compact-eeprom-seven-operation";if(nv)return "parallel-nvram-six-operation";}}
    if(entry.device_class==2034u){lt48_xp_eprom_resources *r=NULL;if(lt48_xp_eprom_open(x,device,&r)==LT48_OK){int fram=lt48_xp_eprom_is_fram(r);lt48_xp_eprom_close(r);if(fram)return "parallel-fram-six-operation";}}
    if(w78e354one_member(&entry)){if(!LT48_W78E354ONE_ADMITTED)return "unclassified";lt48_xp_eprom_resources*r=NULL;if(lt48_xp_eprom_open(x,device,&r)!=LT48_OK)return "unclassified";lt48_xp_eprom_close(r);return "xp-w78eight-seven-operation";}
    if(entry.device_class==2038u){lt48_xp_eprom_resources *r=NULL;if((entry.device_class!=2038u || entry.device_type<0x61 || entry.device_type>0x70 || lt48_w78legacy90_admitted_internal(entry.device_type)) && lt48_xp_eprom_open(x,device,&r)==LT48_OK){int ext=lt48_xp_eprom_extended_fields(r),ra2=lt48_xp_eprom_is_ra2(r),cfg=lt48_xp_eprom_has_configuration(r),rb2=lt48_xp_eprom_is_rb2(r),w78=lt48_xp_eprom_flag_fields(r),philips=lt48_xp_eprom_is_philips(r);lt48_xp_eprom_close(r);return entry.device_type>=0x72&&entry.device_type<=0x75?"xp-w78eight-seven-operation":entry.device_type>=0x61&&entry.device_type<=0x70?"xp-w78-nine-operation":ext?"xp-philips-extended-nine-operation":ra2?"xp-philips-ra2-nine-operation":philips?(w78==4?"xp-philips-x2-nine-operation":"xp-philips-basic-nine-operation"):w78?"xp-w78-nine-operation":rb2?"at89-rb2-nine-operation":cfg?"at89-parallel-nine-operation":"at89-classic-eight-operation";}}
    return lt48_complete_operation_profile(entry.device_class);
}

/* A single live evaluation, with no cached admission decisions. The export
 * caller owns the resulting snapshot; operation owners still revalidate files. */
lt48_status lt48_installation_support_summary(
    const lt48_installation *installation, size_t device_index,
    lt48_operation_support *support, size_t capacity, size_t *out_count,
    const char **out_profile, int *out_complete
) {
    lt48_catalogue_entry entry;
    unsigned mask;
    if (!out_profile || !out_complete || !out_count) return LT48_INVALID_ARGUMENT;
    *out_complete = 0;
    *out_profile = "unclassified";
    lt48_status status = lt48_installation_operation_support(
        installation, device_index, support, capacity, out_count);
    if (status != LT48_OK) return status;
    status = lt48_installation_get_device(installation, device_index, &entry);
    if (status != LT48_OK) return status;
    const char *profile = lt48_installation_complete_operation_profile(installation, device_index);
    *out_profile = profile;
    mask = complete_operation_mask_for_class(entry.device_class);
    if(!strcmp(profile,"sm2965-nine-operation"))mask=0x2ffu;
    if(!strcmp(profile,"mysonM2-seven-operation"))mask=0x7fu;
    if(!strcmp(profile,"myson2-seven-operation")||!strcmp(profile,"myson44-seven-operation"))mask=0xbfu;
    if(!strcmp(profile,"elan5-options-seven-operation"))mask=0x23fu;
    if(!strcmp(profile,"syntek4-options-eight-operation"))mask=0x2bfu;
    if(!strcmp(profile,"scenix6-nine-operation"))mask=0x2ffu;
    if(!strcmp(profile,"psoc-eight-operation"))mask=0xffu;
    if(!strcmp(profile,"cypress-prom-six-operation"))mask=0x3fu;
    if(!strcmp(profile,"lpc-flash-crc-eight-operation"))mask=0x2feu;
    if(!strcmp(profile,"compact40-flash-eight-operation"))mask=0xffu;
    if(!strcmp(profile,"cop48-otp-seven-operation"))mask=0xbfu;
    if(!strcmp(profile,"compact40-otp-seven-operation"))mask=0xbfu;
    if(!strcmp(profile,"pic24-nine-operation"))mask=0x2ffu;
    if(!strcmp(profile,"intel-word-eight-operation"))mask=0xffu;
    if(!strcmp(profile,"mx27page-otp-six-operation"))mask=0x3fu;
    if(!strcmp(profile,"hyundai13-six-operation"))mask=0x3fu;
    if(!strcmp(profile,"hyundai13-options-seven-operation"))mask=0x23fu;
    if(!strcmp(profile,"hyundai13-options-eight-operation")||!strcmp(profile,"st62-options-eight-operation")||!strcmp(profile,"zilog-word-otp-eight-operation"))mask=0x2bfu;
    if(!strcmp(profile,"hyundai-xiprom-seven-operation")||!strcmp(profile,"hyundai-compact-otp-seven-operation")||!strcmp(profile,"st62-otp-seven-operation")||!strcmp(profile,"cypress-usb-otp-seven-operation")||!strcmp(profile,"zilog-otp-seven-operation"))mask=0xbfu;
    if((!strcmp(profile,"tms320e-seven-operation")||!strcmp(profile,"novatek-umc-seven-operation")||!strcmp(profile,"hitachi-pulse-seven-operation")))mask=0xbfu;
    if(!strcmp(profile,"pic-otp-seven-operation"))mask=0xbfu;
    if(!strcmp(profile,"pof-eight-operation"))mask=0xbfu | (1u << (LT48_OPERATION_FUNCTIONAL_TEST-1u));
    if(entry.device_class==2002u && !strcmp(profile,"legacy28f-seven-operation"))mask=0x7fu;
    if((entry.device_class==2007u||entry.device_class==2003u||entry.device_class==2008u||entry.device_class==2004u||entry.device_class==2005u||entry.device_class==2029u) && !strcmp(profile,"legacy-nor-seven-operation"))mask=0x7fu;
    if(!strcmp(profile,"sm2964two-eight-operation"))mask=0xffu;
    if(!strcmp(profile,"white3-seven-operation"))mask=0x7fu;
    if(entry.device_class==2030u && !strcmp(profile,"xp-byte-seven-operation"))mask=0x7f;
    if(lt48_tpi15_binding_internal(&entry))mask=!strcmp(profile,"tpi-seven-operation")?0x7f:0;
    if(lt48_xp13_binding_internal(&entry))mask=!strcmp(profile,"xp-otp-six-operation")?0x3f:!strcmp(profile,"xp-shared-word-seven-operation")?0x7f:0;
    if((entry.device_class==2031u||lt48_lpc16_member_internal(entry.device_class,entry.device_type)||lt48_sharp13_member_internal(entry.device_class,entry.device_type)||lt48_nxp18_member_internal(entry.device_class,entry.device_type)) && !strcmp(profile,"xp-shared-word-seven-operation"))mask=0x7f;
    if((page_byte21_batch(entry.device_class,entry.device_type)||(sst_page_binding(&entry)||at29_binding(&entry)||((emtc18_binding(&entry)&&emtc18_admitted(entry.device_type))||(nand15_binding(&entry)&&nand15_admitted(entry.device_type))))))mask=!strcmp(profile,"page-byte-eight-operation")?0xffu:0x7fu;
    if(entry.device_class==2042u)mask=!strcmp(profile,"page-wide-eight-operation")?0xffu:0x7fu;
    if((entry.device_class==2044u || entry.device_class==2041u) && !strcmp(profile,"addtab-spi-seven-operation"))mask=0x7f;
    if(entry.device_class==2047u && !strcmp(profile,"addtab-otp-seven-operation"))mask=0x7f;
    if(entry.device_class==2045u && !strcmp(profile,"addtab-gl-seven-operation"))mask=0x7f;
    if(entry.device_class==2044u && !strcmp(profile,"addtab-byte-seven-operation"))mask=0x7f;
    if(lt48_eep21_definition_internal(entry.device_class,entry.device_type))mask=0x7fu;
    if(entry.device_class==2006u && entry.device_type==5u)mask=0x7fu;
    if(entry.device_class==2011u)mask=!strcmp(profile,"cypress-prom-six-operation")?0x3fu:0x7fu;
    if(!strcmp(profile,"timekeeper-eight-operation"))mask=0x27f;
    if(entry.device_class==2013u && (entry.device_type>=0x8du && entry.device_type<=0x90u))mask=0x3f;
    if(entry.device_class==2006u && entry.device_type==0xeu)mask=0x31;
    if(entry.device_class==2034u && !strcmp(profile,"serial-otp-five-operation"))mask=0x37u;
    if(entry.device_class==2034u && !strcmp(profile,"gdac-serial-six-operation"))mask=0x77u;
    if(entry.device_class==2009u && (!strcmp(profile,"nexflash-eight-operation")||!strcmp(profile,"w45d-eight-operation")))mask=0x2f7u;
    if(entry.device_class==2009u && !strcmp(profile,"gaaf-serial-six-operation"))mask=0x77u;
    if(entry.device_class==2009u&&entry.device_type>=0x64u&&entry.device_type<=0x68u)mask=(entry.device_type==0x66u||entry.device_type==0x67u)?0x7fu:0x3fu;
    if(entry.device_class==2013u && !strcmp(profile,"hc711-eprom-five-operation"))mask=0x37u;
    if(!strcmp(profile,"hc711-parallel-five-operation"))mask=0x37u;
    if(!strcmp(profile,"hc705-compact-five-operation")||!strcmp(profile,"hc705-bootstrap-five-operation")||!strcmp(profile,"hc705-jp-five-operation")||!strcmp(profile,"hc705-ca-five-operation")||!strcmp(profile,"hc705-bd-five-operation")||!strcmp(profile,"hc705-bx-five-operation")||!strcmp(profile,"hc705-jjsrpl-five-operation"))mask=0x37u;
    if(entry.device_class==2013u && !strcmp(profile,"hc11-ael-seven-operation"))mask=0x277u;
    if(!strcmp(profile,"rs08-six-operation"))mask=0x77u;
    if(!strcmp(profile,"s08-six-operation"))mask=0x77u;
    if(entry.device_class==2013u && !strcmp(profile,"hc11-eeprom-six-operation"))mask=0x77u;
    if(entry.device_class==2014u && (!strcmp(profile,"hc908-fqtqy-six-operation")||!strcmp(profile,"hc908-qtqy-six-operation")||!strcmp(profile,"hc908-jkjl-six-operation")||!strcmp(profile,"hc908-gxjb-six-operation")||!strcmp(profile,"hc908-abasaz-six-operation")||!strcmp(profile,"hc908-gpgz-six-operation")||!strcmp(profile,"hc908-apsr-six-operation")||!strcmp(profile,"hc908-grkx-six-operation")))mask=0x77u;
    if(!strcmp(profile,"hitachi15-five-operation"))mask=0x37u;
    if(!strcmp(profile,"word14-maskrom-four-operation")||!strcmp(profile,"utron4-maskrom-four-operation"))mask=0x33u;
    if(entry.device_class==2009u && (!strcmp(profile,"hitachi-mcu-six-operation")||!strcmp(profile,"fujitsu-mcu-six-operation")))mask=0x77u; /* all six memory operations, no Overprogram/Secure */
    if(entry.device_class==2009u && !strcmp(profile,"legacy-dataflash-six-operation"))mask=0x77u; /* all six memory operations, no Overprogram/Secure */
    if(entry.device_class==2043u){const char *p=profile;mask=!strcmp(p,"at17n-eight-operation")?0x27f:!strcmp(p,"page-i2c-nine-operation")?0x2ff:(!strcmp(p,"spi-page-eight-operation") || !strcmp(p,"page-i2c-eight-operation"))?0xff:0x7f;}
    if(entry.device_class==2012u && !strcmp(profile,"compact-eeprom-seven-operation"))mask=0x7f;
    if(entry.device_class==2005u && !strcmp(profile,"page-eeprom-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"at29word-eight-operation"))mask=0xff;
    if(entry.device_class==2010u && entry.device_type==0xfffe)mask=0x7f;
    if(entry.device_class==2010u && !strcmp(profile,"avr-eight-operation"))mask=0xff;
    if(entry.device_class==2010u && (!strcmp(profile,LT48_FAMILY_XC17_PROFILE)||!strcmp(profile,LT48_FAMILY_SERIAL_PROM_CONFIG_PROFILE)))mask=LT48_FAMILY_XC17_OPERATION_MASK;
    if(entry.device_class==2010u && !strcmp(profile,LT48_FAMILY_SERIAL_PROM_PROFILE))mask=LT48_FAMILY_SERIAL_PROM_OPERATION_MASK;
    if(!strcmp(profile,LT48_FAMILY_ATT_CONFIG_PROFILE))mask=LT48_FAMILY_ATT_CONFIG_OPERATION_MASK;
    if(!strcmp(profile,LT48_FAMILY_XILINX_CONFIG_PROFILE))mask=LT48_FAMILY_XILINX_CONFIG_OPERATION_MASK;
    if(!strcmp(profile,LT48_FAMILY_EPC_OTP_PROFILE))mask=LT48_FAMILY_EPC_OTP_OPERATION_MASK;
    if(!strcmp(profile,LT48_FAMILY_EPC2_PROFILE))mask=LT48_FAMILY_EPC2_OPERATION_MASK;
    if(!strcmp(profile,"atmel-hc-seven-operation"))mask=0x7f;
    if(entry.device_class==2000u && !strcmp(profile,"microwire-seven-operation"))mask|=1u<<(LT48_OPERATION_ERASE-1u);
    if(entry.device_class==2000u && !strcmp(profile,"compact-i2c-seven-operation"))mask|=1u<<(LT48_OPERATION_ERASE-1u);
    if(entry.device_class==2000u && !strcmp(profile,"compact-spi-seven-operation"))mask|=1u<<(LT48_OPERATION_ERASE-1u);
    if(entry.device_class==2000u && !strcmp(profile,"compact-spi-eight-operation"))mask=0xff;
    if(entry.device_class==2000u && !strcmp(profile,"incremental-eight-operation"))mask=0x2bf;
    if(entry.device_class==2000u && (!strcmp(profile,"idlock-nine-operation") || !strcmp(profile,"compact-flash-nine-operation")))mask=0x2ff;
    if(entry.device_class==2000u && !strcmp(profile,"compact-i2c-eight-operation"))mask=0xff;
    if(entry.device_class==2000u && !strcmp(profile,"endurance-nine-operation"))mask=0x2ff;
    if(entry.device_class==2000u && !strcmp(profile,"at17-compact-eight-operation"))mask=0x27f;
    if(entry.device_class==2000u && !strcmp(profile,"spd-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"tmp1962one-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"tms87two-six-operation")||!strcmp(profile,"tms370seven-six-operation"))mask=0x3f;
    if(!strcmp(profile,"r8c8-nine-operation")||!strcmp(profile,"st7twelve-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"ite2-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"sierra2-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"ptc010three-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"gdacotp6-six-operation"))mask=0x3f;
    if(!strcmp(profile,"amic4-otp-six-operation"))mask=0x3f;
    if(!strcmp(profile,"at91nine-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"ob16-eight-operation"))mask=0x2bf;
    if(!strcmp(profile,"so30-eight-operation"))mask=0x2bf;
    if(!strcmp(profile,"wt10-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"wt8-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"ab11-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"hy14-eight-operation"))mask=0x2bf;
    if(!strcmp(profile,"mw6-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"my7-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"my4-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"my4-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"sc2-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"sc4g-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"sc4a-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"tp4-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"tp6-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"mw8-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"gc8-seven-operation"))mask=0xbf;
    if(!strcmp(profile,"su16-eight-operation"))mask=0x2bf;
    if(!strcmp(profile,"sm3-eight-operation"))mask=0xff;
    if(!strcmp(profile,"sm6-eight-operation"))mask=0xff;
    if(!strcmp(profile,"sn9-nine-operation")||!strcmp(profile,"mg15-nine-operation")||!strcmp(profile,"sx30-nine-operation")||!strcmp(profile,"sm30-nine-operation")||!strcmp(profile,"mw36-nine-operation")||!strcmp(profile,"sy76-nine-operation")||!strcmp(profile,"sy80-nine-operation")||!strcmp(profile,"mw27-nine-operation")||!strcmp(profile,"mf74-nine-operation")||!strcmp(profile,"ramtron6-nine-operation")||!strcmp(profile,"t89six-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"at28c64b1-seven-operation")||!strcmp(profile,"eep2041-three-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"flash51-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"stm8s4-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"cc254x5-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"snas4-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"novatek25-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"xp-w78eight-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"at89-classic-eight-operation")||!strcmp(profile,"flash51-eight-operation"))mask=0xff;
    if(!strcmp(profile,"at89-compact-eight-operation") || !strcmp(profile,"at89-legacy-lv-eight-operation"))mask=0xff;
    if(!strcmp(profile,"legacy-mcu20-options-eight-operation"))mask=0x27fu;
    if(!strcmp(profile,"legacy-mcu20-flash-nine-operation"))mask=0x2ffu;
    if(!strcmp(profile,"legacy-mcu20-flash-eight-operation"))mask=0xffu;
    if(!strcmp(profile,"legacy-mcu20-otp-eight-operation"))mask=0x2bfu;
    if(!strcmp(profile,"legacy-mcu20-otp-seven-operation"))mask=0xbfu;
    if(!strcmp(profile,"legacy-mcu20-six-operation"))mask=0x3fu;
    if(!strcmp(profile,"stk11c68-seven-operation"))mask=0x7fu;
    if(!strcmp(profile,"otp13five-six-operation"))mask=0x3fu;
    if(!strcmp(profile,"legacy14-otp-seven-operation"))mask=0xbfu;
    if(!strcmp(profile,"legacy14-nvram-eight-operation"))mask=0xffu;
    if(!strcmp(profile,"aduc812-seven-operation"))mask=0x7fu;
    if(!strcmp(profile,"legacy14-flash-seven-operation"))mask=0x7fu;
    if(!strcmp(profile,"legacy14-aduc-nine-operation"))mask=0x2ffu;
    if(!strcmp(profile,"nuvoton8-nine-operation") || !strcmp(profile,"xmega7-nine-operation") || !strcmp(profile,"sstmx16-nine-operation") || !strcmp(profile,"n79w79_21-nine-operation") || !strcmp(profile,"nuvoton30-nine-operation") || !strcmp(profile,"msp43076-nine-operation") || !strcmp(profile,"numicro81-nine-operation") || !strcmp(profile,"n79w79-nine-operation") || lt48_configuration_fields_profile(profile) || !strcmp(profile,"at89-ic2-nine-operation") || !strcmp(profile,"at89-re2-nine-operation") || (!strcmp(profile,"at89-s8253-nine-operation") || !strcmp(profile,"p89v51-nine-operation") || (!strcmp(profile,"sst89rd-nine-operation") || (!strcmp(profile,"w78d24-nine-operation") || !strcmp(profile,"w78bank147-nine-operation")))) || !strcmp(profile,"at89-s2051-nine-operation") || !strcmp(profile,"at89-lp51-basic-nine-operation") || !strcmp(profile,"at89-lp2052-nine-operation") || !strcmp(profile,"at89-usb51-nine-operation") || !strcmp(profile,"at89-ed2-nine-operation") || !strcmp(profile,"at89-lp51-nine-operation") || !strcmp(profile,"at89-lp-large-nine-operation") || !strcmp(profile,"at89-lp21-nine-operation") || !strcmp(profile,"at89-legacy-s-nine-operation") || (!strcmp(profile,"xp-philips-extended-nine-operation") || !strcmp(profile,"xp-philips-ra2-nine-operation")) || !strcmp(profile,"xp-philips-x2-nine-operation") || !strcmp(profile,"xp-philips-basic-nine-operation") || !strcmp(profile,"xp-w78-nine-operation") || !strcmp(profile,"at89-parallel-nine-operation") || !strcmp(profile,"at89-rb2-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"cl38-memory-seven-operation"))mask=0x7f;
    if(!strcmp(profile,"cl38-typed-eight-operation"))mask=0x2bf;
    if(!strcmp(profile,"cl38-typed-nine-operation"))mask=0x2ff;
    if(!strcmp(profile,"philips39-eight-operation") || !strcmp(profile,"philips-lpc-eight-operation") || !strcmp(profile,"upi41ah-eight-operation"))mask=0x2bf;
    if(!strcmp(profile,"xc7000-seven-operation") || !strcmp(profile,"philips39-seven-operation"))mask=0xbf;
    if(entry.device_class==2019u) {
        lt48_device_spec selected;
        if(lt48_installation_resolve_device(installation,device_index,&selected)==LT48_OK && pz20_binding(&selected))mask|=1u<<(LT48_OPERATION_ERASE-1u);
    }
    if(entry.device_class==2022u) {
        lt48_device_spec selected;
        if(lt48_installation_resolve_device(installation,device_index,&selected)==LT48_OK && (pls26_binding(&selected)||epx6_binding(&selected))) {
            lt48_pal_resources*r=NULL;char path[4096];uint16_t op;
            snprintf(path,sizeof path,"%s/GBBA___.rez",lt48_installation_path(installation));
            if(lt48_pal_resources_open_definition(path,selected.definition_resource,selected.family_code,selected.pinmap,&r)==LT48_OK) {
                if(lt48_pal_resources_operation(r,"_FUN_SECU",&op)!=LT48_OK)mask&=~(1u<<(LT48_OPERATION_SECURE-1u));
                lt48_pal_resources_close(r);
            }
        }
        if(lt48_installation_resolve_device(installation,device_index,&selected)==LT48_OK &&
           compact20_binding(&selected) && selected.family_code>=25u && selected.family_code<=28u)
            mask&=~(1u<<(LT48_OPERATION_SECURE-1u));
    }
    if (!mask) return LT48_OK;
    for (unsigned operation = 1u;
         operation <= LT48_OPERATION_KIND_COUNT; ++operation) {
        if ((mask & (1u << (operation - 1u))) == 0u) continue;
        if (operation > *out_count || support[operation - 1u].operation !=
                (lt48_operation_kind)operation ||
            support[operation - 1u].availability !=
                LT48_OPERATION_EXECUTABLE)
            return LT48_OK;
    }
    *out_complete = 1;
    return LT48_OK;
}

int lt48_installation_device_complete_support(
    const lt48_installation *installation, size_t device_index
) {
    lt48_operation_support support[LT48_OPERATION_KIND_COUNT];
    size_t count;
    const char *profile;
    int complete = 0;
    return lt48_installation_support_summary(installation, device_index,
        support, LT48_OPERATION_KIND_COUNT, &count, &profile, &complete) == LT48_OK
        && complete;
}

static int variable_name_equal(
    const unsigned char *name, size_t length, const char *expected
) {
    size_t expected_length = strlen(expected);
    if (name == NULL || length != expected_length) return 0;
    for (size_t index = 0u; index < length; ++index) {
        if (tolower(name[index]) !=
            tolower((unsigned char)expected[index])) return 0;
    }
    return 1;
}

static int verify_vcc_name_index(
    const unsigned char *name, size_t length, unsigned *out_index
) {
    static const char prefix[] = "verifyvcc";
    if (name == NULL || out_index == NULL || length != sizeof(prefix)) return 0;
    for (size_t index = 0u; index < sizeof(prefix) - 1u; ++index) {
        if (tolower(name[index]) != prefix[index]) return 0;
    }
    if (name[sizeof(prefix) - 1u] < '0' ||
        name[sizeof(prefix) - 1u] > '3') return 0;
    *out_index = (unsigned)(name[sizeof(prefix) - 1u] - '0');
    return 1;
}

lt48_status lt48_installation_verify_voltage_profile(
    const lt48_installation *installation, size_t device_index,
    unsigned margin_type, lt48_verify_voltage_profile *out_profile
) {
    lt48_catalogue_entry entry;
    lt48_device_spec spec;
    lt48_family_info family;
    lt48_rez *rez = NULL;
    const char *stem;
    char *path = NULL;
    char family_name[16];
    size_t family_index;
    size_t names_index;
    uint16_t explicit_values[4] = {0u,0u,0u,0u};
    int explicit_present[4] = {0,0,0,0};
    uint16_t nominal = 0u;
    unsigned nominal_named_count = 0u;
    uint16_t fallback_nominal = 0u;
    unsigned fallback_count = 0u;
    lt48_status status;
    if (installation == NULL || out_profile == NULL || margin_type > 1u)
        return LT48_INVALID_ARGUMENT;
    memset(out_profile, 0, sizeof(*out_profile));
    status = lt48_installation_get_device(installation, device_index, &entry);
    if (status == LT48_OK)
        status = lt48_installation_resolve_device(
            installation, device_index, &spec
        );
    if (status != LT48_OK) return status;
    stem = installation_definition_stem(installation, entry.device_class);
    if (stem == NULL || strlen(installation->path) >
        SIZE_MAX - strlen(stem) - 6u) return LT48_NOT_FOUND;
    path = malloc(strlen(installation->path) + strlen(stem) + 6u);
    if (path == NULL) return LT48_NO_MEMORY;
    snprintf(path, strlen(installation->path) + strlen(stem) + 6u,
             "%s/%s.rez", installation->path, stem);
    status = lt48_rez_open(path, &rez);
    free(path);
    if (status != LT48_OK) return status;
    snprintf(family_name, sizeof(family_name), "M%x", spec.family_code);
    status = lt48_rez_find_resource(rez, family_name, &family_index);
    if (status == LT48_OK)
        status = lt48_rez_get_family_info(rez, family_index, &family);
    if (status == LT48_OK)
        status = lt48_rez_find_resource(rez, "VarName", &names_index);
    for (size_t index = 0u; status == LT48_OK && index < family.variable_count;
         ++index) {
        lt48_family_variable variable;
        const unsigned char *name = NULL;
        size_t name_length = 0u;
        unsigned explicit_index;
        status = lt48_rez_get_family_variable(
            rez, family_index, index, &variable
        );
        if (status != LT48_OK) break;
        status = lt48_rez_get_variable_name(
            rez, names_index, variable.values[1], &name, &name_length, NULL
        );
        if (status != LT48_OK) break;
        if (variable.values[2] == 0u || variable.values[3] == 0u) continue;
        if (variable.values[1] == 0u) {
            fallback_nominal = variable.values[3];
            ++fallback_count;
        }
        if (variable_name_equal(name, name_length, "VCC")) {
            nominal = variable.values[3];
            ++nominal_named_count;
        }
        if (verify_vcc_name_index(name, name_length, &explicit_index)) {
            if (explicit_present[explicit_index]) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            explicit_present[explicit_index] = 1;
            explicit_values[explicit_index] = variable.values[3];
        }
    }
    lt48_rez_close(rez);
    if (status != LT48_OK) return status;
    if (nominal_named_count == 0u && fallback_count == 1u)
        nominal = fallback_nominal;
    if (nominal_named_count > 1u ||
        (nominal_named_count == 0u && fallback_count != 1u) ||
        nominal < 100u || nominal > 700u) return LT48_SAFETY_REJECTED;
    if(!strcmp(lt48_installation_source_profile(installation),"Dataman 13.10")&&lt48_eep21_read_nominal_internal(&spec))nominal=330;
    out_profile->nominal_centivolts = nominal;
    out_profile->margin_type = (uint8_t)margin_type;
    for (unsigned index = 0u; index < 4u; ++index) {
        if (!explicit_present[index]) continue;
        if (explicit_values[index] < 100u || explicit_values[index] > 700u)
            return LT48_SAFETY_REJECTED;
        out_profile->explicit_values_centivolts[
            out_profile->explicit_value_count++
        ] = explicit_values[index];
    }
    if (out_profile->explicit_value_count != 0u) {
        out_profile->low_centivolts =
            out_profile->explicit_values_centivolts[0];
        out_profile->high_centivolts = out_profile->explicit_values_centivolts[
            out_profile->explicit_value_count - 1u
        ];
    } else if (margin_type == 0u) {
        out_profile->low_centivolts = (uint16_t)(nominal * 95u / 100u);
        out_profile->high_centivolts = (uint16_t)(nominal * 105u / 100u);
    } else {
        out_profile->low_centivolts = (uint16_t)(nominal * 90u / 100u);
        out_profile->high_centivolts = (uint16_t)(nominal * 110u / 100u);
    }
    if (out_profile->low_centivolts > nominal ||
        out_profile->high_centivolts < nominal) return LT48_SAFETY_REJECTED;
    return LT48_OK;
}

lt48_status lt48_verify_voltage_profile_select(
    const lt48_verify_voltage_profile *profile,
    lt48_verify_margin_selection selection,
    uint16_t *voltages, size_t capacity, size_t *out_count
) {
    uint16_t selected[3];
    size_t count = 0u;
    if (profile == NULL || out_count == NULL ||
        (voltages == NULL && capacity != 0u) ||
        selection > LT48_VERIFY_MARGIN_ALL) return LT48_INVALID_ARGUMENT;
    if (selection == LT48_VERIFY_MARGIN_LOW ||
        selection == LT48_VERIFY_MARGIN_ALL)
        selected[count++] = profile->low_centivolts;
    if (selection == LT48_VERIFY_MARGIN_NOMINAL ||
        selection == LT48_VERIFY_MARGIN_ALL) {
        if (count == 0u || selected[count - 1u] != profile->nominal_centivolts)
            selected[count++] = profile->nominal_centivolts;
    }
    if (selection == LT48_VERIFY_MARGIN_HIGH ||
        selection == LT48_VERIFY_MARGIN_ALL) {
        if (count == 0u || selected[count - 1u] != profile->high_centivolts)
            selected[count++] = profile->high_centivolts;
    }
    *out_count = count;
    if (capacity < count || (count != 0u && voltages == NULL))
        return LT48_BUFFER_TOO_SMALL;
    memcpy(voltages, selected, count * sizeof(*voltages));
    return LT48_OK;
}

static int auto_id_candidate_compare(const void *left_value,
                                     const void *right_value) {
    const lt48_auto_id_candidate *left = left_value;
    const lt48_auto_id_candidate *right = right_value;
    if (left->score != right->score) return left->score > right->score ? -1 : 1;
    if (left->device_index == right->device_index) return 0;
    return left->device_index < right->device_index ? -1 : 1;
}

lt48_status lt48_installation_rank_auto_id(
    const lt48_installation *installation,
    const lt48_auto_id_observation *observation,
    lt48_auto_id_candidate *items, size_t capacity, size_t *out_count
) {
    lt48_auto_id_candidate *candidates;
    size_t candidate_count = 0u;
    if (installation == NULL || observation == NULL || out_count == NULL ||
        (items == NULL && capacity != 0u)) return LT48_INVALID_ARGUMENT;
    *out_count = 0u;
    if (!observation->has_manufacturer_id && !observation->has_device_id &&
        !observation->has_pin_count_hint &&
        !observation->has_device_class_hint) return LT48_INVALID_ARGUMENT;
    if (installation->device_count > SIZE_MAX / sizeof(*candidates))
        return LT48_FORMAT_ERROR;
    candidates = calloc(
        installation->device_count == 0u ? 1u : installation->device_count,
        sizeof(*candidates)
    );
    if (candidates == NULL) return LT48_NO_MEMORY;
    for (size_t index = 0u; index < installation->device_count; ++index) {
        lt48_catalogue_entry entry;
        lt48_auto_id_candidate *candidate;
        lt48_device_spec spec;
        lt48_status status = lt48_installation_get_device(
            installation, index, &entry
        );
        if (status != LT48_OK) {
            free(candidates);
            return status;
        }
        if (observation->has_manufacturer_id &&
            entry.manufacturer_id != observation->manufacturer_id) continue;
        if (observation->has_device_id &&
            entry.device_id != observation->device_id) continue;
        candidate = &candidates[candidate_count++];
        candidate->device_index = index;
        candidate->catalogue = entry;
        if (observation->has_manufacturer_id) {
            candidate->manufacturer_id_match = 1;
            candidate->score += 1000u;
        }
        if (observation->has_device_id) {
            candidate->device_id_match = 1;
            candidate->score += 1000u;
        }
        if (observation->has_device_class_hint &&
            entry.device_class == observation->device_class_hint) {
            candidate->device_class_match = 1;
            candidate->score += 50u;
        }
        if (observation->has_pin_count_hint &&
            lt48_installation_resolve_device(
                installation, index, &spec) == LT48_OK) {
            candidate->has_resolved_pin_count = 1;
            candidate->resolved_pin_count = spec.pin_count;
            if ((uint16_t)spec.pin_count == observation->pin_count_hint) {
                candidate->pin_count_match = 1;
                candidate->score += 100u;
            }
        }
    }
    qsort(candidates, candidate_count, sizeof(*candidates),
          auto_id_candidate_compare);
    *out_count = candidate_count;
    if (candidate_count > capacity || (candidate_count != 0u && items == NULL)) {
        free(candidates);
        return LT48_BUFFER_TOO_SMALL;
    }
    if (candidate_count != 0u)
        memcpy(items, candidates, candidate_count * sizeof(*candidates));
    free(candidates);
    return LT48_OK;
}

lt48_status lt48_installation_rank_auto_id_eprom_execution(
    const lt48_installation *installation,
    const lt48_auto_id_eprom_execution_result *execution,
    lt48_auto_id_candidate *items, size_t capacity, size_t *out_count
) {
    lt48_auto_id_observation observation;
    lt48_status status;
    if (installation == NULL || execution == NULL || out_count == NULL ||
        (items == NULL && capacity != 0u))
        return LT48_INVALID_ARGUMENT;
    status = lt48_auto_id_eprom_execution_observation(
        execution, &observation);
    if (status != LT48_OK) {
        *out_count = 0u;
        return status;
    }
    return lt48_installation_rank_auto_id(
        installation, &observation, items, capacity, out_count);
}

/* A library record derives from an exact imported resource tuple. Runtime
 * engines still apply their own operation admission and electrical checks. */
static int new_record_matches(const lt48_new_device_record *r,const lt48_device_spec *s) {
    return s->catalogue.device_class==r->base_class && s->definition_resource==r->base_definition &&
        s->family_code==r->base_family && s->pinmap==r->base_pinmap &&
        s->pin_count==r->base_pins && s->capacity_bytes==r->base_capacity;
}
static lt48_status installation_load_new_library(lt48_installation *x) {
    char root[4096];lt48_status st=lt48_new_device_lib_default_path(root,sizeof root);
    if(st==LT48_NOT_FOUND)return LT48_OK;
    if(st!=LT48_OK)return st;
    st=lt48_new_device_lib_open(root,&x->new_library,&x->new_report);if(st!=LT48_OK)return st;
    size_t count=lt48_new_device_lib_count(x->new_library);
    x->new_devices=calloc(count?count:1,sizeof *x->new_devices);
    x->corrections=calloc(x->device_count?x->device_count:1,sizeof *x->corrections);
    if(!x->new_devices||!x->corrections)return LT48_NO_MEMORY;
    for(size_t j=0;j<count;j++) {
        const lt48_new_device_record *r=lt48_new_device_lib_record(x->new_library,j);
        if(!strcmp(r->status,"experimental"))continue;
        const char *stem=installation_definition_stem(x,r->base_class);
        char expected[32];snprintf(expected,sizeof expected,"%s.rez",stem?stem:"");
        if(strcmp(expected,r->base_file))continue; /* Different source layout. */
        char path[4096],hex[65];unsigned char hash[32];lt48_rez *rez=NULL;
        if(snprintf(path,sizeof path,"%s/%s",x->path,r->base_file)>=(int)sizeof path)return LT48_FORMAT_ERROR;
        st=lt48_rez_open(path,&rez);if(st==LT48_OK)st=lt48_rez_sha256(rez,hash);lt48_rez_close(rez);
        if(st!=LT48_OK)continue;
        for(unsigned k=0;k<32;k++)snprintf(hex+2*k,3,"%02x",hash[k]);
        if(strcmp(hex,r->base_sha256))continue; /* Library is version-specific. */
        for(size_t i=0;i<x->device_count;i++) {
            lt48_catalogue_entry e;if(lt48_installation_get_device(x,i,&e)!=LT48_OK)return LT48_FORMAT_ERROR;
            if(e.device_class!=r->base_class || e.device_type!=r->base_definition)continue;
            char key[288];snprintf(key,sizeof key,"%s:%.*s",e.vendor,(int)e.device_name_length,e.device_name);
            int correction=!strcmp(r->kind,"correction");
            if(strcmp(r->base_key,key)&&!(correction&&!strcmp(r->base_key,"*")))continue;
            lt48_device_spec spec;st=lt48_installation_resolve_device(x,i,&spec);
            if(st!=LT48_OK||!new_record_matches(r,&spec))continue;
            if(correction) {
                if(x->corrections[i])return LT48_FORMAT_ERROR;
                x->corrections[i]=r;
            } else {
                for(size_t k=0;k<lt48_installation_device_count(x);k++) {
                    lt48_catalogue_entry old;lt48_installation_get_device(x,k,&old);
                    if(!strcmp(old.vendor,r->manufacturer)&&old.device_name_length==strlen(r->part)&&!memcmp(old.device_name,r->part,old.device_name_length))return LT48_FORMAT_ERROR;
                }
                x->new_devices[x->new_count].record=r;x->new_devices[x->new_count++].donor=i;
                break;
            }
        }
    }
    return LT48_OK;
}
const char *lt48_installation_new_device_lib_path(const lt48_installation *x){return x&&x->new_library?x->new_report.root:NULL;}
const char *lt48_installation_new_device_lib_sha256(const lt48_installation *x){return x&&x->new_library?x->new_report.sha256:NULL;}
const lt48_new_device_record *lt48_installation_new_device_record(const lt48_installation *x,size_t device) {
    if(!x)return NULL;
    if(device<x->device_count)return x->corrections?x->corrections[device]:NULL;
    device-=x->device_count;return device<x->new_count?x->new_devices[device].record:NULL;
}
lt48_status lt48_installation_apply_new_device_variables(const lt48_installation *x,size_t device,lt48_compile_variable *vars,size_t count) {
    if(!x||!vars||device>=lt48_installation_device_count(x))return LT48_INVALID_ARGUMENT;
    const lt48_new_device_record *r=lt48_installation_new_device_record(x,device);
    if(device>=x->device_count && device-x->device_count<x->new_count) {
        /* A derived device also inherits corrections to its imported base. */
        size_t donor=x->new_devices[device-x->device_count].donor;
        if(x->corrections[donor])r=x->corrections[donor];
    }
    if(!r)return LT48_OK;
    /* Validate the complete delta before modifying any variable. */
    for(size_t j=0;j<r->variable_count;j++) {
        unsigned found=0;
        for(size_t i=0;i<count;i++)if(vars[i].variable_id==r->variables[j].id) {
            if(vars[i].kind!=r->variables[j].kind || ((uint32_t)vars[i].value|((uint32_t)vars[i].channel<<16))!=r->variables[j].expected)return LT48_SAFETY_REJECTED;
            found++;
        }
        if(found!=1)return LT48_SAFETY_REJECTED;
    }
    for(size_t j=0;j<r->variable_count;j++)for(size_t i=0;i<count;i++)if(vars[i].variable_id==r->variables[j].id) {
        vars[i].value=r->variables[j].value&65535;vars[i].channel=r->variables[j].value>>16;
    }
    return LT48_OK;
}
