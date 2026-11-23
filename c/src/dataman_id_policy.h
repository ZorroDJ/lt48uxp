#ifndef LT48_DATAMAN_ID_POLICY_H
#define LT48_DATAMAN_ID_POLICY_H

#include <stdint.h>

/* Dataman 13.10, selector-zero branch at preferred PE 004506B0.
 * Caller must separately establish that GETID is bound and capture succeeded.
 * Catalogue and observed IDs are WORDs; do not truncate them to bytes.
 */
static inline int lt48_dataman_selector_zero_id_matches(
    uint16_t expected_manufacturer, uint16_t expected_device,
    uint16_t observed_manufacturer, uint16_t observed_device
) {
    if (expected_manufacturer == 0u && expected_device == 0u) {
        return observed_manufacturer == observed_device &&
               (observed_manufacturer == 0u ||
                observed_manufacturer == 0xffu);
    }
    return expected_manufacturer == observed_manufacturer &&
           expected_device == observed_device;
}

#endif
