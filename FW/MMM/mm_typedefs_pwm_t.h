/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_pwm_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_PWM_T_H
#define MM_TYPEDEFS_PWM_T_H

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
/** @brief Controls and calculates PWM values */
typedef union {
    struct {
        basic_mode_t mode; /**< basic mode for pwm settings */
        basic_gpio_t dut_pwm;
        uint16_t duty_cycle; /**< The calculated duty cycle in percent/100 */
        uint32_t period; /**< The calculated period in us */
        uint32_t h_ticks; /**< Settable high time in sys clock ticks */
        uint32_t l_ticks; /**< Settable low time in sys clock ticks */
        uint8_t padding[16]; /**< padding bytes */
    };
    uint8_t data[32]; /**< Array for padding */
} pwm_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_PWM_T_H */
/** @} **/