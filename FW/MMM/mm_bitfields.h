/**
 ******************************************************************************
 * @addtogroup bitfields_mmm
 * @{
 * @file      mm_bitfields.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_BITFIELDS_H
#define MM_BITFIELDS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"

/* bitfield typedefs *********************************************************/
MM_PACKED_START
/** @brief ADC mode settings */
typedef union {
    struct {
        uint8_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint8_t disable: 1; /**< 0:periph is enabled - 1:periph is disabled */
        uint8_t fast_sample: 1; /**< 0:slow sample rate - 1:fast sample rate */
        uint8_t padding: 5; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} adc_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief IO pin mode control */
typedef union {
    struct {
        uint16_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint16_t io_type: 2; /**< 0:high impedance input - 1:push pull output - 2:open drain output - 3:interrupts and saves event */
        uint16_t level: 1; /**< If output sets gpio level - 0:low - 1:high */
        uint16_t pull: 2; /**< pull of the resistor - 0:none - 1:pullup - 2:pulldown */
        uint16_t tick_div: 5; /**< for trace tick divisor - max should be 16 for interface */
        uint16_t padding: 5; /**< padding bits */
    };
    uint16_t data; /**< Full bitfield data */
} gpio_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief Simplified GPIO for periph GPIO control */
typedef union {
    struct {
        uint8_t io_type: 2; /**< 0:high impedance input - 1:push pull output - 2:open drain output */
        uint8_t pull: 2; /**< pull of the resistor - 0:none - 1:pullup - 2:pulldown */
        uint8_t set_level: 1; /**< If output sets gpio level - 0:low - 1:high */
        uint8_t level: 1; /**< Current value of gpio - 0:low - 1:high */
        uint8_t padding: 2; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} basic_gpio_t;
MM_PACKED_END

MM_PACKED_START
/** @brief IO pin status */
typedef union {
    struct {
        uint8_t level: 1; /**< The io level of the pin - 0:low - 1:high */
        uint8_t padding: 7; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} gpio_status_t;
MM_PACKED_END

MM_PACKED_START
/** @brief I2C mode settings */
typedef union {
    struct {
        uint8_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint8_t disable: 1; /**< 0:periph is enabled - 1:periph is disabled */
        uint8_t addr_10_bit: 1; /**< 0:i2c address is 7 bit mode - 1:10 i2c address is 10 bit mode */
        uint8_t general_call: 1; /**< 0:disable general call - 1:enable general call */
        uint8_t no_clk_stretch: 1; /**< 0:slave can clock stretch - 1:disables clock stretch */
        uint8_t reg_16_bit: 1; /**< 0:8 bit register access - 1:16 bit register access mode */
        uint8_t reg_16_big_endian: 1; /**< 0:little endian if 16 bit register access - 1:big endian if 16 bit register access */
        uint8_t nack_data: 1; /**< 0:all data will ACK - 1:all data will NACK */
    };
    uint8_t data; /**< Full bitfield data */
} i2c_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief I2C status */
typedef union {
    struct {
        uint8_t ovr: 1; /**< Overrun/Underrun: Request for new byte when not ready */
        uint8_t af: 1; /**< Acknowledge failure */
        uint8_t berr: 1; /**< Bus error:  Non-valid position during a byte transfer */
        uint8_t gencall: 1; /**< General call address received */
        uint8_t busy: 1; /**< i2c bus is BUSY */
        uint8_t rsr: 1; /**< Repeated start detected */
        uint8_t padding: 2; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} i2c_status_t;
MM_PACKED_END

MM_PACKED_START
/** @brief Basic mode settings */
typedef union {
    struct {
        uint8_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint8_t disable: 1; /**< 0:periph is enabled - 1:periph is disabled */
        uint8_t padding: 6; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} basic_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief SPI status */
typedef union {
    struct {
        uint8_t bsy: 1; /**< Busy flag */
        uint8_t ovr: 1; /**< Overrun flag */
        uint8_t modf: 1; /**< Mode fault */
        uint8_t udr: 1; /**< Underrun flag */
        uint8_t clk: 1; /**< 0:sclk line low - 1:sclk line high */
        uint8_t start_clk: 1; /**< SCLK reading at start of frame - 0:sclk line low - 1:sclk line high */
        uint8_t end_clk: 1; /**< SCLK reading at end of frame - 0:sclk line low - 1:sclk line high */
        uint8_t index_err: 1; /**< Register index error */
    };
    uint8_t data; /**< Full bitfield data */
} spi_status_t;
MM_PACKED_END

MM_PACKED_START
/** @brief SPI mode settings */
typedef union {
    struct {
        uint8_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint8_t disable: 1; /**< 0:periph is enabled - 1:periph is disabled */
        uint8_t cpha: 1; /**< 0:CK to 0 when idle - 1:CK to 1 when idle */
        uint8_t cpol: 1; /**< 0:the first clock transition is the first data capture edge - 1:the second clock transition is the first data capture edge */
        uint8_t if_type: 2; /**< Sets spi modes since slave cannot responds immediately - 0:access registers with spi - 1:preloads reg address to 0 for high speed tests - 2:echos SPI bytes - 3:always output user reg 0 (use for timing) */
        uint8_t reg_16_bit: 1; /**< 0:8 bit register access - 1:16 bit register access mode */
        uint8_t reg_16_big_endian: 1; /**< 0:little endian for 16 bit mode - 1:big endian for 16 bit mode */
    };
    uint8_t data; /**< Full bitfield data */
} spi_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief System mode settings */
typedef union {
    struct {
        uint8_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint8_t dut_rst: 1; /**< 1:put DUT in reset mode - 0:run DUT */
        uint8_t padding: 6; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} sys_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief System status */
typedef union {
    struct {
        uint8_t update: 1; /**< 1:register configuration requires execution for changes - 0:nothing to update */
        uint8_t board: 1; /**< 1:board is a bluepill - 0:board is a nucleo-f103rb */
        uint8_t padding: 6; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} sys_status_t;
MM_PACKED_END

MM_PACKED_START
/** @brief Timer mode settings */
typedef union {
    struct {
        uint8_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint8_t disable: 1; /**< 0:periph is enabled - 1:periph is disabled */
        uint8_t trig_edge: 2; /**< 0:both - 1:rising - 2:falling */
        uint8_t padding: 4; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} tmr_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief UART mode settings */
typedef union {
    struct {
        uint16_t init: 1; /**< 0:periph will initialize on execute - 1:periph initialized */
        uint16_t disable: 1; /**< 0:periph is enabled - 1:periph is disabled */
        uint16_t if_type: 2; /**< 0:echos - 1:echos and adds one - 2:reads application registers - 3:constantly transmits */
        uint16_t stop_bits: 1; /**< 0:1 stop bit - 1:2 stop bits */
        uint16_t parity: 2; /**< 0:no parity - 1:even parity - 2:odd parity */
        uint16_t rts: 1; /**< RTS pin state */
        uint16_t data_bits: 1; /**< 0:8 data bits - 1:7 data bits */
        uint16_t padding: 7; /**< padding bits */
    };
    uint16_t data; /**< Full bitfield data */
} uart_mode_t;
MM_PACKED_END

MM_PACKED_START
/** @brief UART status */
typedef union {
    struct {
        uint8_t cts: 1; /**< CTS pin state */
        uint8_t pe: 1; /**< Parity error */
        uint8_t fe: 1; /**< Framing error */
        uint8_t nf: 1; /**< Noise detected flag */
        uint8_t ore: 1; /**< Overrun error */
        uint8_t padding: 3; /**< padding bits */
    };
    uint8_t data; /**< Full bitfield data */
} uart_status_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_BITFIELDS_H */
/** @} **/