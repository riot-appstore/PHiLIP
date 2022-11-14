/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_dac_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_DAC_T_H
#define MM_TYPEDEFS_DAC_T_H

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
/** @brief Controls DAC values */
typedef union {
    struct {
        basic_mode_t mode; /**< basic mode for dac settings */
        basic_gpio_t dut_dac;
        uint16_t level; /**< The percent/100 of output level */
        uint8_t padding[12]; /**< padding bytes */
    };
    uint8_t data[16]; /**< Array for padding */
} dac_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_DAC_T_H */
/** @} **/