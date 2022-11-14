/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_uart_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_UART_T_H
#define MM_TYPEDEFS_UART_T_H

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
/** @brief Controls and provides information for the uart */
typedef union {
    struct {
        uart_mode_t mode; /**< UART mode register */
        basic_gpio_t dut_rx;
        basic_gpio_t dut_tx;
        basic_gpio_t dut_cts;
        basic_gpio_t dut_rts;
        uint32_t baud; /**< Baudrate */
        uint8_t mask_msb; /**< Masks the data coming in if 7 bit mode */
        uint16_t rx_count; /**< Number of received bytes */
        uint16_t tx_count; /**< Number of transmitted bytes */
        uart_status_t status; /**< UART status register */
    };
    uint8_t data[16]; /**< Array for padding */
} uart_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_UART_T_H */
/** @} **/