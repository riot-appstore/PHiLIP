/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_tmr_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_TMR_T_H
#define MM_TYPEDEFS_TMR_T_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"
#include "mm_bitfields.h"

/* tyepdefs ******************************************************************/
MM_PACKED_START
/** @brief Controls timer input capture values */
typedef union {
    struct {
        tmr_mode_t mode; /**< basic mode for dac settings */
        basic_gpio_t dut_ic;
        uint16_t min_holdoff; /**< The minimum amount of time to wait before triggering another event in ns */
        uint32_t min_tick; /**< Minimum tick difference */
        uint32_t max_tick; /**< Maximum tick difference */
    };
    uint8_t data[12]; /**< Array for padding */
} tmr_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_TMR_T_H */
/** @} **/