/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_spi_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_SPI_T_H
#define MM_TYPEDEFS_SPI_T_H

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
/** @brief Controls and provides information for the spi */
typedef union {
    struct {
        spi_mode_t mode; /**< Specific spi modes */
        spi_status_t status; /**< Spi status register */
        basic_gpio_t dut_mosi;
        basic_gpio_t dut_miso;
        basic_gpio_t dut_sck;
        basic_gpio_t dut_nss;
        uint16_t state; /**< Current state of the spi bus - 0:initialized - 1:NSS pin just lowered - 2:writing to reg - 3:reading reg - 4:transferring data - 5:NSS up and finished */
        uint16_t reg_index; /**< current index of reg pointer */
        uint16_t start_reg_index; /**< start index of reg pointer */
        uint8_t r_count; /**< Last read frame byte count */
        uint8_t w_count; /**< Last write frame byte count */
        uint8_t transfer_count; /**< The amount of bytes in the last transfer  */
        uint32_t frame_ticks; /**< Ticks per frame */
        uint32_t byte_ticks; /**< Ticks per byte */
        uint32_t prev_ticks; /**< Holder for previous byte ticks */
        uint32_t sm_buf[64]; /**< Buffer for captured timestamps for speed measurement */
    };
    uint8_t data[283]; /**< Array for padding */
} spi_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_SPI_T_H */
/** @} **/