#ifndef LT48_PIC18_J_OWNER_H
#define LT48_PIC18_J_OWNER_H
#include <labtool48/labtool48.h>
#include "../../NewDeviceLib/sources/pic-common/pic18_j_options.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_j_adapters.h"
#include "../../NewDeviceLib/sources/pic-common/source_transport.h"
typedef struct lt48_pic18_j_owner lt48_pic18_j_owner;
enum {LT48_PIC18_J_ERASE_AUTHORIZED=1,LT48_PIC18_J_IGNORE_ID_MISMATCH=2};
typedef struct {
 lt48_small_eprom_result operation;
 lt48_pic_contact_result contact;
 uint16_t observed_id;int id_mismatch,protected_read,configuration_from_mirrors,backup_valid;
} lt48_pic18_j_owner_result;
/* Private exact-alias operator owner, not admission. All published availability
 * stays with the normal public dispatcher until the complete batch is sealed. */
lt48_status lt48_pic18_j_owner_open(const lt48_installation *,size_t,lt48_pic18_j_owner **);
void lt48_pic18_j_owner_close(lt48_pic18_j_owner *);
const lt48_pic18_j_options_profile *lt48_pic18_j_owner_options(const lt48_pic18_j_owner *);
const lt48_pic18_j_adapter_profile *lt48_pic18_j_owner_adapter(const lt48_pic18_j_owner *);
lt48_status lt48_pic18_j_owner_contact_masks(const lt48_pic18_j_owner *,lt48_pic_contact_masks *);
/* Read, Blank, Compare, Verify or Erase. This entry uses nominal voltage;
 * the separate Verify entry selects audited explicit source margin levels. Wrong ID is
 * reported explicitly; these private requests never imply destructive consent. */
lt48_status lt48_pic18_j_owner_memory(const lt48_pic18_j_owner *,
 const lt48_protocol_transport *,void *,lt48_operation_kind,int,
 const uint8_t *,size_t,uint8_t *,size_t,lt48_pic18_j_owner_result *);
void lt48_pic18_j_owner_set_id_decision(lt48_pic18_j_owner *,lt48_m37_id_decision_fn,void *);
lt48_status lt48_pic18_j_owner_verify(const lt48_pic18_j_owner *,
 const lt48_protocol_transport *,void *,int,const uint8_t *,size_t,
 lt48_verify_margin_selection,unsigned,lt48_pic18_j_owner_result *);
/* Program/Over Program/Secure use explicit erase consent. Secure on an already
 * protected part is idempotent. Preservation backups stay in caller storage. */
lt48_status lt48_pic18_j_owner_mutate(const lt48_pic18_j_owner *,
 const lt48_protocol_transport *,void *,lt48_operation_kind,int,
 const uint8_t *,size_t,uint8_t *,size_t,lt48_pic18_j_owner_result *);

#endif
