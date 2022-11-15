/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_i2c_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_I2C_T_H
#define MM_TYPEDEFS_I2C_T_H

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
/** @brief System settings for the device */
typedef union {
    struct {
        i2c_mode_t mode; /**< Specific modes for I2C */
        i2c_status_t status; /**< Specific status for I2C */
        basic_gpio_t dut_sda;
        basic_gpio_t dut_scl;
        uint16_t clk_stretch_delay; /**< Clock stretch the first byte in us */
        uint16_t slave_addr_1; /**< Primary slave address */
        uint16_t slave_addr_2; /**< Secondary slave address */
        uint16_t state; /**< Current state of i2c frame - 0:initialized - 1:reading data - 2-write address received - 3-1st reg byte received - 4-writing data - 5-NACK - 6-stopped */
        uint16_t reg_index; /**< Current index of reg pointer */
        uint16_t start_reg_index; /**< Start index of reg pointer */
        uint8_t r_count; /**< Last read frame byte count - only in reg if_type 0 */
        uint8_t w_count; /**< Last write frame byte count - only in reg if_type 0 */
        uint32_t r_ticks; /**< Ticks for read byte - only in reg if_type 0 */
        uint32_t w_ticks; /**< Ticks for write byte */
        uint32_t s_ticks; /**< Holder when the start occurred */
        uint32_t f_r_ticks; /**< Ticks for full read frame after the address is acked */
        uint32_t f_w_ticks; /**< Ticks for full write frame */
        uint8_t padding[26]; /**< padding bytes */
    };
    uint8_t data[64]; /**< Array for padding */
} i2c_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_I2C_T_H */
/** @} **/