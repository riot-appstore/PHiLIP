/**
 ******************************************************************************
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_typedef.h
 * @author    Kevin Weiss
 * @version   0.0.2
 * @date      2019-03-22
 * @details   Generated from the memory map manager
 ******************************************************************************
 */

#ifndef PHILIP_TYPEDEF_H
#define PHILIP_TYPEDEF_H


#include <stdint.h>

#pragma pack(1)
/** @brief  System mode settings */
typedef struct {
	uint8_t init : 1; /**< initialize with new params */
	uint8_t dut_rst : 1; /**< resets the DUT */
} sys_mode_t;

/** @brief  System status */
typedef struct {
	uint8_t update : 1; /**< Update available execute needed */
} sys_status_t;

/** @brief  I2C mode settings */
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

/** @brief  I2C status */
typedef struct {
	uint8_t ovr : 1; /**< Overrun/Underrun: Request for new byte when not ready */
	uint8_t af : 1; /**< Acknowledge failure */
	uint8_t berr : 1; /**< Bus error:  Non-valid position during a byte transfer */
	uint8_t gencall : 1; /**< General call address received */
	uint8_t busy : 1; /**< i2c bus is BUSY */
	uint8_t rsr : 1; /**< Repeated start detected */
} i2c_status_t;

/** @brief  SPI mode settings */
typedef struct {
	uint8_t init : 1; /**< initialize with new params */
	uint8_t disable : 1; /**< disable spi functionality */
	uint8_t cpha : 1; /**< Clock phase */
	uint8_t cpol : 1; /**< Clock polarity */
	uint8_t if_type : 2; /**< spi interface type */
	uint8_t reg_16_bit : 1; /**< 16 bit register access mode */
	uint8_t reg_16_big_endian : 1; /**< sets the endianess of the 16 bit reg */
} spi_mode_t;

/** @brief  SPI status */
typedef struct {
	uint8_t bsy : 1; /**< Busy flag */
	uint8_t ovr : 1; /**< Overrun flag */
	uint8_t modf : 1; /**< Mode fault */
	uint8_t udr : 1; /**< Underrun flag */
	uint8_t clk : 1; /**< Current reading of spi clock */
	uint8_t start_clk : 1; /**< Clk reading when cs pin starts spi frame */
	uint8_t end_clk : 1; /**< Clk reading when cs pin ends frame */
	uint8_t index_err : 1; /**< Register index error */
} spi_status_t;

/** @brief  UART mode settings */
typedef struct {
	uint16_t init : 1; /**< initialize with new params */
	uint16_t disable : 1; /**< disable uart functionality */
	uint16_t if_type : 2; /**< interface mode/echo/etc. */
	uint16_t stop_bits : 1; /**< Number of stop bits */
	uint16_t parity : 2; /**< Parity */
	uint16_t rts : 1; /**< RTS pin state */
	uint16_t data_bits : 1; /**< Number of data bits */
} uart_mode_t;

/** @brief  UART status */
typedef struct {
	uint8_t cts : 1; /**< CTS pin state */
	uint8_t pe : 1; /**< Parity error */
	uint8_t fe : 1; /**< Framing error */
	uint8_t nf : 1; /**< Noise detected flag */
	uint8_t ore : 1; /**< Overrun error */
} uart_status_t;

/** @brief  IO pin mode control */
typedef struct {
	uint16_t init : 1; /**< initialize with new params */
	uint16_t io_type : 2; /**< direction of io, in/outpp/outod/int */
	uint16_t level : 1; /**< 0 for low, 1 for high, only if output */
	uint16_t pull : 2; /**< pull of the resistor none/up/down */
	uint16_t tick_div : 5; /**< for trace tick divisor */
} gpio_mode_t;

/** @brief  IO pin status */
typedef struct {
	uint8_t level : 1; /**< The io level of the pin, 0=low, 1=high */
} gpio_status_t;

/** @brief  ADC mode settings */
typedef struct {
	uint8_t init : 1; /**< initialize with new params */
	uint8_t enable : 1; /**< enables periph functionality (disabled by default) */
	uint8_t fast_sample : 1; /**< Set to sample as fast as possible */
} adc_mode_t;

/** @brief  Basic mode settings */
typedef struct {
	uint8_t init : 1; /**< initialize with new params */
	uint8_t disable : 1; /**< disable periph functionality */
} basic_mode_t;

/** @brief  Time and date */
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

/** @brief  System settings for PHiLIP */
typedef union {
	struct {
		uint8_t sn[12]; /**< Unique ID of the device */
		uint8_t fw_rev[4]; /**< Firmware revision */
		uint8_t if_rev[4]; /**< Interface revision */
		uint64_t tick; /**< Tick in ms */
		timestamp_t build_time; /**< time of build */
		uint32_t device_num; /**< A constant number that should always be the same */
		uint32_t sys_clk; /**< The frequency of the system clock */
		sys_status_t status; /**< Status of system */
		sys_mode_t mode; /**< Control register for device */
		uint8_t res[18]; /**< Reserved bytes */
	};
	uint8_t data8[64];/**< array for padding */
} sys_t;

/** @brief  System settings for the device */
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
		uint32_t s_ticks; /**< Holder when the start occured */
		uint32_t f_r_ticks; /**< Ticks for full read frame */
		uint32_t f_w_ticks; /**< Ticks for full write frame */
		uint8_t res[28]; /**< Reserved bytes */
	};
	uint8_t data8[64];/**< array for padding */
} i2c_t;

/** @brief  Controls and provides information for the spi */
typedef union {
	struct {
		spi_mode_t mode; /**< Specific spi modes */
		spi_status_t status; /**< Spi status register */
		uint16_t state; /**< Current state of the i2c bus */
		uint16_t reg_index; /**< current index of i2c pointer */
		uint16_t start_reg_index; /**< start index of i2c pointer */
		uint8_t r_count; /**< Last read frame byte count */
		uint8_t w_count; /**< Last write frame byte count */
		uint8_t transfer_count; /**< The amount of bytes in the last transfer  */
		uint32_t frame_ticks; /**< Ticks per frame */
		uint32_t byte_ticks; /**< Ticks per byte */
		uint32_t prev_ticks; /**< Holder for previous byte ticks */
		uint8_t res[9]; /**< Reserved bytes */
	};
	uint8_t data8[32];/**< array for padding */
} spi_t;

/** @brief  Controls and provides information for the uart */
typedef union {
	struct {
		uart_mode_t mode; /**< UART mode register */
		uint32_t baud; /**< Baudrate */
		uint8_t mask_msb; /**< Baudrate */
		uint16_t rx_count; /**< Number of received bytes */
		uint16_t tx_count; /**< Number of transmitted bytes */
		uart_status_t status; /**< UART status register */
		uint8_t res[4]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} uart_t;

/** @brief  Controls and reads the ADC */
typedef union {
	struct {
		adc_mode_t mode; /**< Mode settings for the ADC */
		uint32_t num_of_samples; /**< Number of sample in the sum */
		uint8_t counter; /**< sum counter, increases when available */
		uint32_t index; /**< sample index, increases when new sample read */
		uint16_t sample; /**< current 12 bit sample value */
		uint32_t sum; /**< Sum of the last num_of_samples */
		uint32_t current_sum; /**< Current collection of the sums */
		uint8_t res[12]; /**< Reserved bytes */
	};
	uint8_t data8[32];/**< array for padding */
} adc_t;

/** @brief  Controls and calculates PWM values */
typedef union {
	struct {
		basic_mode_t mode; /**< basic mode for pwm settings */
		uint8_t status; /**< Unimplemented status for padding */
		uint16_t duty_cycle; /**< The calculated duty cycle in percent/100 */
		uint32_t period; /**< The calculated period in ticks */
		uint32_t h_ticks; /**< Settable high time in sys clock ticks */
		uint32_t l_ticks; /**< Settable low time in sys clock ticks */
		uint8_t res[16]; /**< Reserved bytes */
	};
	uint8_t data8[32];/**< array for padding */
} pwm_t;

/** @brief  Controls DAC values */
typedef union {
	struct {
		basic_mode_t mode; /**< basic mode for dac settings */
		uint8_t status; /**< Unimplemented status for padding */
		uint16_t level; /**< The percent/100 of output level */
		uint8_t res[12]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} dac_t;

/** @brief  Controls the RTC */
typedef union {
	struct {
		basic_mode_t mode; /**< basic mode for rtc settings */
		uint8_t second; /**< Seconds of rtc */
		uint8_t minute; /**< Minutes of rtc */
		uint8_t hour; /**< Hours of rtc */
		uint16_t day; /**< Days of rtc */
		uint8_t set_second; /**< Seconds to set of rtc */
		uint8_t set_minute; /**< Minutes to set of rtc */
		uint8_t set_hour; /**< Hours to set of rtc */
		uint16_t set_day; /**< Days to set of rtc */
		uint8_t res[5]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} rtc_t;

/** @brief  unimplemented TMR */
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

/** @brief  Controls GPIO settings */
typedef union {
	struct {
		gpio_mode_t mode; /**< The selected GPIO mode */
		gpio_status_t status; /**< The status of the GPIO */
		uint8_t res[1]; /**< Reserved bytes */
	};
	uint8_t data8[4];/**< array for padding */
} gpio_t;

/** @brief  Saved timestamps and events */
typedef union {
	struct {
		uint32_t index; /**< Index of the current trace */
		uint8_t tick_div[32]; /**< The tick divisor of the event */
		uint8_t source[32]; /**< The event source of the event */
		uint16_t value[32]; /**< The value of the event */
		uint32_t tick[32]; /**< The tick when the event occured */
	};
	uint8_t data8[260];/**< array for padding */
} trace_t;

/** @brief  The memory map */
typedef union {
	struct {
		uint8_t user_reg[256]; /**< Writable registers for user testing */
		sys_t sys; /**< System configuration (protected) */
		i2c_t i2c; /**< I2C configuration */
		spi_t spi; /**< SPI configuration */
		uart_t uart; /**<  */
		rtc_t rtc; /**< RTC configuration */
		adc_t adc; /**< ADC configuration */
		pwm_t pwm; /**< PWM configuration */
		dac_t dac; /**< DAC configuration */
		tmr_t tmr; /**<  */
		gpio_t gpio[3]; /**< GPIO pins available */
		trace_t trace; /**< Saved timestamps and events */
		uint8_t res[208]; /**< Reserved bytes */
	};
	uint8_t data8[1024];/**< array for padding */
} map_t;

#pragma pack()
#endif /* PHILIP_TYPEDEF_H */
/** @} */
