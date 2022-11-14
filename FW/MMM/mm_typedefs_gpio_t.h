/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_gpio_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_GPIO_T_H
#define MM_TYPEDEFS_GPIO_T_H

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
/** @brief Controls GPIO settings */
typedef union {
    struct {
        gpio_mode_t mode; /**< The selected GPIO mode */
        gpio_status_t status; /**< The status of the GPIO */
        uint8_t padding[1]; /**< padding bytes */
    };
    uint8_t data[4]; /**< Array for padding */
} gpio_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_GPIO_T_H */
/** @} **/