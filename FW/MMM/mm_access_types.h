/**
 ******************************************************************************
 * @addtogroup mmm
 * @{
 * @file      mm_access_types.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_ACCESS_TYPES_H
#define MM_ACCESS_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"

/* typedefs ******************************************************************/
MM_PACKED_START
/** @brief  System settings for the device */
typedef union {
    struct {
        uint8_t write_permission: 4; /**< bitmasked permission users for writes */
        uint8_t read_permission: 4; /**< bitmasked permission users for reads */
    };
    uint8_t data;
} mm_access_t;
MM_PACKED_END

/* defs **********************************************************************/
#define MM_ACCESS_INTERFACE 1
#define MM_ACCESS_PERIPHERAL 2

#ifdef __cplusplus
}
#endif

#endif /* MM_ACCESS_TYPES_H */
/** @} **/