/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_map_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_MAP_T_H
#define MM_TYPEDEFS_MAP_T_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"
#include "mm_typedefs_adc_t.h"
#include "mm_typedefs_dac_t.h"
#include "mm_typedefs_gpio_t.h"
#include "mm_typedefs_i2c_t.h"
#include "mm_typedefs_pwm_t.h"
#include "mm_typedefs_rtc_t.h"
#include "mm_typedefs_spi_t.h"
#include "mm_typedefs_sys_t.h"
#include "mm_typedefs_tmr_t.h"
#include "mm_typedefs_trace_t.h"
#include "mm_typedefs_uart_t.h"

/* tyepdefs ******************************************************************/
MM_PACKED_START
/** @brief The memory map */
typedef union {
    struct {
        uint8_t user_reg[256]; /**< Writable registers for user testing - Starts at 0 and increases 1 each register by default */
        sys_t sys; /**< System configuration (protected) */
        i2c_t i2c; /**< I2C configuration */
        spi_t spi; /**< SPI configuration */
        uart_t uart;
        rtc_t rtc; /**< RTC configuration */
        adc_t adc; /**< ADC configuration */
        pwm_t pwm; /**< PWM configuration */
        dac_t dac; /**< DAC configuration */
        tmr_t tmr; /**< TMR configuration */
        gpio_t gpio[3]; /**< GPIO pins available */
        trace_t trace; /**< Saved timestamps and events */
    };
    uint8_t data[1813]; /**< Array for padding */
} map_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_MAP_T_H */
/** @} **/