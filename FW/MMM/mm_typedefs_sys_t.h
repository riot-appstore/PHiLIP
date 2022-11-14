/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_sys_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_SYS_T_H
#define MM_TYPEDEFS_SYS_T_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"
#include "mm_bitfields.h"
#include "mm_typedefs_timestamp_t.h"

/* tyepdefs ******************************************************************/
MM_PACKED_START
/** @brief System settings for PHiLIP */
typedef union {
    struct {
        uint8_t sn[12]; /**< Unique ID of the device */
        uint8_t fw_rev[4]; /**< Firmware revision */
        uint8_t if_rev[4]; /**< Interface revision - This correlates to the version of the memory map */
        uint64_t tick; /**< Tick in ms - Updates with the sys tick register every few ms */
        timestamp_t build_time; /**< time of build */
        uint32_t device_num; /**< The frequency of the system clock in Hz */
        uint32_t sys_clk;
        sys_status_t status; /**< Status of system */
        sys_mode_t mode; /**< Control register for device */
    };
    uint8_t data[46]; /**< Array for padding */
} sys_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_SYS_T_H */
/** @} **/