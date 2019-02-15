/**
 ******************************************************************************
 * @file       : PHiLIP_typedef.h
 * @author     : Kevin Weiss
 * @version    : 0.00.01
 * @date       : 2019-02-13
 * @addtogroup : PHiLIP_Typedefs
 * @{
 ******************************************************************************
 */

#ifndef PHILIP_TYPEDEF_H
#define PHILIP_TYPEDEF_H

#include <stdint.h>

#pragma pack(1)
/** @brief  :  */
typedef struct {
	uint8_t init : 1; /**< initialize with new params */
	uint8_t dut_rst : 1; /**< resets the DUT */
} sys_mode_t;

/** @brief  :  */
typedef struct {
	uint8_t update : 1; /**< Update available execute needed */
} sys_status_t;

/** @brief  :  */
typedef struct {
	uint8_t init : 1; /**< initialize with new params */
	uint8_t disable : 1; /**< disable i2c functionality */
	uint8_t addr_10_bit : 1; /**< 10 bit address enable */
	uint8_t general_call : 1; /**< General call enable */
	uint8_t no_clk_stretch : 1; /**< Disable for clk stretch */
	uint8_t reg_16_bit : 1; /**< 16 bit register access mode */
	uint8_t reg_16_big_endian : 1; /**< sets the endianess of the 16 bit reg */
	uint8_t nack_data : 1; /**< Forces a data nack */
} i2c_mode_t;

/** @brief  :  */
typedef struct {
	uint8_t ovr : 1; /**< Overrun/Underrun: Request for new byte when not ready */
	uint8_t af : 1; /**< Acknowledge failure */
	uint8_t berr : 1; /**< Bus error:  Non-valid position during a byte transfer */
	uint8_t gencall : 1; /**< General call address received */
	uint8_t busy : 1; /**< i2c bus is BUSY */
	uint8_t rsr : 1; /**< Repeated start detected */
} i2c_status_t;

/** @brief  :  */
typedef struct {
	uint16_t init : 1; /**< initialize with new params */
	uint16_t disable : 1; /**< disable uart functionality */
	uint16_t if_type : 2; /**< interface mode/echo/etc. */
	uint16_t stop_bits : 1; /**< Number of stop bits */
	uint16_t parity : 2; /**< Parity */
	uint16_t rts : 1; /**< RTS pin state */
	uint16_t data_bits : 1; /**< Number of data bits */
} uart_mode_t;

/** @brief  :  */
typedef struct {
	uint8_t cts : 1; /**< CTS pin state */
	uint8_t pe : 1; /**< Parity error */
	uint8_t fe : 1; /**< Framing error */
	uint8_t nf : 1; /**< Noise detected flag */
	uint8_t ore : 1; /**< Overrun error */
} uart_status_t;

/** @brief  : Time and date */
typedef union {
	struct {
		uint8_t second; /**< The seconds in decimal */
		uint8_t minute; /**< The minutes in decimal */
		uint8_t hour; /**< The hours in decimal */
		uint8_t day_of_month; /**< The months in decimal */
		uint8_t day_of_week; /**< The week in decimal */
		uint8_t month; /**< The month in decimal */
		uint8_t year; /**< The last two digits of the year in decimal (20xx) */
		uint8_t res[1]; /**< Reserved bytes */
	};
	uint8_t data8[8];/**< array for padding */
} timestamp_t;

/** @brief  : System settings for PHiLIP */
typedef union {
	struct {
		uint8_t sn[12]; /**< Unique ID of the device */
		uint32_t fw_rev; /**< Firmware revision */
		uint32_t if_rev; /**< Interface revision */
		uint64_t tick; /**< Tick in ms */
		timestamp_t build_time; /**< time of build */
		uint32_t device_num; /**< A constant number that should always be the same */
		sys_status_t status; /**< Status of system */
		sys_mode_t mode; /**< Control register for device */
		uint8_t res[22]; /**< Reserved bytes */
	};
	uint8_t data8[64];/**< array for padding */
} sys_t;

/** @brief  : System settings for the device */
typedef union {
	struct {
		i2c_mode_t mode; /**< Specific modes for I2C */
		i2c_status_t status; /**< Specific status for I2C */
		uint16_t clk_stretch_delay; /**< Delay in us for clock stretch */
		uint16_t slave_addr_1; /**< Primary slave address */
		uint16_t slave_addr_2; /**< Secondary slave address */
		uint16_t state; /**< Current state of the i2c bus */
		uint16_t reg_index; /**< current index of i2c pointer */
		uint16_t start_reg_index; /**< start index of i2c pointer */
		uint8_t r_count; /**< Last read frame byte count */
		uint8_t w_count; /**< Last write frame byte count */
		uint32_t r_ticks; /**< Ticks for read byte */
		uint32_t w_ticks; /**< Ticks for write byte */
		uint32_t s_ticks; /**< Ticks for start and address */
		uint32_t f_r_ticks; /**< Ticks for full read frame */
		uint32_t f_w_ticks; /**< Ticks for full write frame */
		uint8_t res[28]; /**< Reserved bytes */
	};
	uint8_t data8[64];/**< array for padding */
} i2c_t;

/** @brief  :  */
typedef union {
	struct {
		uint8_t mode; /**<  */
		uint32_t error_code; /**<  */
		uint8_t res[11]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} spi_t;

/** @brief  : Controls and provides information for the uart */
typedef union {
	struct {
		uart_mode_t mode; /**< UART control register */
		uint32_t baud; /**< Baudrate */
		uint8_t mask_msb; /**< Baudrate */
		uint16_t rx_count; /**< Number of received bytes */
		uint16_t tx_count; /**< Number of transmitted bytes */
		uart_status_t status; /**< UART status register */
		uint8_t res[4]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} uart_t;

/** @brief  :  */
typedef union {
	struct {
		uint8_t mode; /**<  */
		uint16_t error_code; /**<  */
		uint8_t sample_rate; /**<  */
		uint32_t value; /**<  */
		uint8_t res[8]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} adc_t;

/** @brief  :  */
typedef union {
	struct {
		uint8_t mode; /**<  */
		uint16_t error_code; /**<  */
		uint8_t duty; /**<  */
		uint32_t freq; /**<  */
		uint8_t res[8]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} pwm_t;

/** @brief  :  */
typedef union {
	struct {
		uint8_t mode; /**<  */
		uint16_t error_code; /**<  */
		uint8_t duty; /**<  */
		uint32_t freq; /**<  */
		uint32_t hi_us; /**<  */
		uint32_t lo_us; /**<  */
	};
	uint8_t data8[16];/**< array for padding */
} tmr_t;

/** @brief  : The memory map */
typedef union {
	struct {
		uint8_t user_reg[256]; /**< Writable registers for user testing */
		sys_t sys; /**< System configuration (protected) */
		i2c_t i2c; /**< I2C configuration */
		spi_t spi; /**<  */
		uart_t uart; /**<  */
		timestamp_t rtc; /**<  */
		adc_t adc[2]; /**<  */
		pwm_t pwm; /**<  */
		tmr_t tmr; /**<  */
		uint8_t res[536]; /**< Reserved bytes */
	};
	uint8_t data8[1024];/**< array for padding */
} map_t;

#pragma pack()
#endif /* PHILIP_TYPEDEF_H */
/** @} */
