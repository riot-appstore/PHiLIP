/**
 *****************************************************************************"
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_typedef.h
 * @author    Kevin Weiss
 * @version   1.3.0
 * @details   Generated from the memory map manager version 1.0.0
 *****************************************************************************"
 */

#ifndef PHILIP_TYPEDEF_H
#define PHILIP_TYPEDEF_H


#include <stdint.h>

#pragma pack(1)
/** @brief  System mode settings */
typedef struct {
	uint8_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint8_t dut_rst : 1; /**< 1:put DUT in reset mode - 0:run DUT */
	uint8_t res : 6; /**< Reserved bits */
} sys_mode_t;

/** @brief  System status */
typedef struct {
	uint8_t update : 1; /**< 1:register configuration requires execution for changes - 0:nothing to update */
	uint8_t board : 1; /**< 1:board is a bluepill - 0:board is a nucleo-f103rb */
	uint8_t res : 6; /**< Reserved bits */
} sys_status_t;

/** @brief  I2C mode settings */
typedef struct {
	uint8_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint8_t disable : 1; /**< 0:periph is enabled - 1:periph is disabled */
	uint8_t addr_10_bit : 1; /**< 0:i2c address is 7 bit mode - 1:10 i2c address is 10 bit mode */
	uint8_t general_call : 1; /**< 0:disable general call - 1:enable general call */
	uint8_t no_clk_stretch : 1; /**< 0:slave can clock stretch - 1:disables clock stretch */
	uint8_t reg_16_bit : 1; /**< 0:8 bit register access - 1:16 bit register access mode */
	uint8_t reg_16_big_endian : 1; /**< 0:little endian if 16 bit register access - 1:big endian if 16 bit register access */
	uint8_t nack_data : 1; /**< 0:all data will ACK - 1:all data will NACK */
} i2c_mode_t;

/** @brief  I2C status */
typedef struct {
	uint8_t ovr : 1; /**< Overrun/Underrun: Request for new byte when not ready */
	uint8_t af : 1; /**< Acknowledge failure */
	uint8_t berr : 1; /**< Bus error:  Non-valid position during a byte transfer */
	uint8_t gencall : 1; /**< General call address received */
	uint8_t busy : 1; /**< i2c bus is BUSY */
	uint8_t rsr : 1; /**< Repeated start detected */
	uint8_t res : 2; /**< Reserved bits */
} i2c_status_t;

/** @brief  SPI mode settings */
typedef struct {
	uint16_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint16_t disable : 1; /**< 0:periph is enabled - 1:periph is disabled */
	uint16_t cpha : 1; /**< 0:CK to 0 when idle - 1:CK to 1 when idle */
	uint16_t cpol : 1; /**< 0:the first clock transition is the first data capture edge - 1:the second clock transition is the first data capture edge */
	uint16_t if_type : 3; /**< Sets spi modes since slave cannot repsond immediately - 0:access registers with spi - 1:preloads reg address to 0 for high speed tests - 2:echos SPI bytes - 3:always output user reg 0 (use for timing) - 4:use input capture to measure the clock speed */
	uint16_t reg_16_bit : 1; /**< 0:8 bit register access - 1:16 bit register access mode */
	uint16_t reg_16_big_endian : 1; /**< 0:little endian for 16 bit mode - 1:big endian for 16 bit mode */
	uint16_t res : 7; /**< Reserved bits */
} spi_mode_t;

/** @brief  SPI status */
typedef struct {
	uint8_t bsy : 1; /**< Busy flag */
	uint8_t ovr : 1; /**< Overrun flag */
	uint8_t modf : 1; /**< Mode fault */
	uint8_t udr : 1; /**< Underrun flag */
	uint8_t clk : 1; /**< 0:sclk line low - 1:sclk line high */
	uint8_t start_clk : 1; /**< SCLK reading at start of frame - 0:sclk line low - 1:sclk line high */
	uint8_t end_clk : 1; /**< SCLK reading at end of frame - 0:sclk line low - 1:sclk line high */
	uint8_t index_err : 1; /**< Register index error */
} spi_status_t;

/** @brief  UART mode settings */
typedef struct {
	uint16_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint16_t disable : 1; /**< 0:periph is enabled - 1:periph is disabled */
	uint16_t if_type : 2; /**< 0:echos - 1:echos and adds one - 2:reads application registers - 3:constantly transmits */
	uint16_t stop_bits : 1; /**< 0:1 stop bit - 1:2 stop bits */
	uint16_t parity : 2; /**< 0:no parity - 1:even parity - 2:odd parity */
	uint16_t rts : 1; /**< RTS pin state */
	uint16_t data_bits : 1; /**< 0:8 data bits - 1:7 data bits */
	uint16_t res : 7; /**< Reserved bits */
} uart_mode_t;

/** @brief  UART status */
typedef struct {
	uint8_t cts : 1; /**< CTS pin state */
	uint8_t pe : 1; /**< Parity error */
	uint8_t fe : 1; /**< Framing error */
	uint8_t nf : 1; /**< Noise detected flag */
	uint8_t ore : 1; /**< Overrun error */
	uint8_t res : 3; /**< Reserved bits */
} uart_status_t;

/** @brief  IO pin mode control */
typedef struct {
	uint16_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint16_t io_type : 2; /**< 0:high impedance input - 1:push pull output - 2:open drain output - 3:interrupts and saves event */
	uint16_t level : 1; /**< If output sets gpio level - 0:low - 1:high */
	uint16_t pull : 2; /**< pull of the resistor - 0:none - 1:pullup - 2:pulldown */
	uint16_t tick_div : 5; /**< for trace tick divisor - max should be 16 for interface */
	uint16_t res : 5; /**< Reserved bits */
} gpio_mode_t;

/** @brief  Simplified GPIO for periph GPIO control */
typedef struct {
	uint8_t io_type : 2; /**< 0:high impedance input - 1:push pull output - 2:open drain output */
	uint8_t pull : 2; /**< pull of the resistor - 0:none - 1:pullup - 2:pulldown */
	uint8_t set_level : 1; /**< If output sets gpio level - 0:low - 1:high */
	uint8_t level : 1; /**< Current value of gpio - 0:low - 1:high */
	uint8_t res : 2; /**< Reserved bits */
} basic_gpio_t;

/** @brief  IO pin status */
typedef struct {
	uint8_t level : 1; /**< The io level of the pin - 0:low - 1:high */
	uint8_t res : 7; /**< Reserved bits */
} gpio_status_t;

/** @brief  ADC mode settings */
typedef struct {
	uint8_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint8_t disable : 1; /**< 0:periph is enabled - 1:periph is disabled */
	uint8_t fast_sample : 1; /**< 0:slow sample rate - 1:fast sample rate */
	uint8_t res : 5; /**< Reserved bits */
} adc_mode_t;

/** @brief  Timer mode settings */
typedef struct {
	uint8_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint8_t disable : 1; /**< 0:periph is enabled - 1:periph is disabled */
	uint8_t trig_edge : 2; /**< 0:both - 1:rising - 2:falling */
	uint8_t res : 4; /**< Reserved bits */
} tmr_mode_t;

/** @brief  Basic mode settings */
typedef struct {
	uint8_t init : 1; /**< 0:periph will initialize on execute - 1:periph initialized */
	uint8_t disable : 1; /**< 0:periph is enabled - 1:periph is disabled */
	uint8_t res : 6; /**< Reserved bits */
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
		uint8_t if_rev[4]; /**< Interface revision - This correlates to the version of the memory map */
		uint64_t tick; /**< Tick in ms - Updates with the sys tick register every few ms */
		timestamp_t build_time; /**< time of build */
		uint32_t device_num; /**< The philip device designator - A constant number to identify philip firmware */
		uint32_t sys_clk; /**< The frequency of the system clock in Hz */
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
		basic_gpio_t dut_sda; /**<  */
		basic_gpio_t dut_scl; /**<  */
		uint16_t clk_stretch_delay; /**< Clock stretch the first byte in us */
		uint16_t slave_addr_1; /**< Primary slave address */
		uint16_t slave_addr_2; /**< Secondary slave address */
		uint16_t state; /**< Current state of the spi bus - 0:initialized - 1:NSS pin just lowered - 2:writing to reg - 3:reading reg - 4:transferring data - 5:NSS up and finished */
		uint16_t reg_index; /**< Current index of reg pointer */
		uint16_t start_reg_index; /**< Start index of reg pointer */
		uint8_t r_count; /**< Last read frame byte count */
		uint8_t w_count; /**< Last write frame byte count - only in reg if_type 0 */
		uint32_t r_ticks; /**< Ticks for read byte - only in reg if_type 0 */
		uint32_t w_ticks; /**< Ticks for write byte */
		uint32_t s_ticks; /**< Holder when the start occurred */
		uint32_t f_r_ticks; /**< Ticks for full read frame after the address is acked */
		uint32_t f_w_ticks; /**< Ticks for full write frame */
		uint8_t res[26]; /**< Reserved bytes */
	};
	uint8_t data8[64];/**< array for padding */
} i2c_t;

/** @brief  Controls and provides information for the spi */
typedef union {
	struct {
		spi_mode_t mode; /**< Specific spi modes */
		spi_status_t status; /**< Spi status register */
		basic_gpio_t dut_mosi; /**<  */
		basic_gpio_t dut_miso; /**<  */
		basic_gpio_t dut_sck; /**<  */
		basic_gpio_t dut_nss; /**<  */
		uint16_t state; /**< Current state of i2c frame - 0:initialized - 1:reading data - 2-write address received - 3-1st reg byte received - 4-writing data - 5-NACK - 6-stopped */
		uint16_t reg_index; /**< current index of i2c pointer */
		uint16_t start_reg_index; /**< start index of i2c pointer */
		uint8_t r_count; /**< Last read frame byte count */
		uint8_t w_count; /**< Last write frame byte count */
		uint8_t transfer_count; /**< The amount of bytes in the last transfer  */
		uint32_t frame_ticks; /**< Ticks per frame */
		uint32_t byte_ticks; /**< Ticks per byte */
		uint32_t prev_ticks; /**< Holder for previous byte ticks */
		uint32_t sm_buf[64]; /**< Buffer for captured timestamps for speed measurement */
	};
	uint8_t data8[284];/**< array for padding */
} spi_t;

/** @brief  Controls and provides information for the uart */
typedef union {
	struct {
		uart_mode_t mode; /**< UART mode register */
		basic_gpio_t dut_rx; /**<  */
		basic_gpio_t dut_tx; /**<  */
		basic_gpio_t dut_cts; /**<  */
		basic_gpio_t dut_rts; /**<  */
		uint32_t baud; /**< Baudrate */
		uint8_t mask_msb; /**< Masks the data coming in if 7 bit mode */
		uint16_t rx_count; /**< Number of received bytes */
		uint16_t tx_count; /**< Number of transmitted bytes */
		uart_status_t status; /**< UART status register */
	};
	uint8_t data8[16];/**< array for padding */
} uart_t;

/** @brief  Controls and reads the ADC */
typedef union {
	struct {
		adc_mode_t mode; /**< Mode settings for the ADC */
		basic_gpio_t dut_adc; /**<  */
		uint32_t num_of_samples; /**< Number of sample in the sum */
		uint32_t index; /**< Sample index increases when new sample read */
		uint16_t sample; /**< Current 12 bit sample value */
		uint32_t sum; /**< Sum of the last num_of_samples */
		uint32_t current_sum; /**< Current collection of the sums */
		uint8_t counter; /**< Sum counter increases when available */
		uint8_t res[11]; /**< Reserved bytes */
	};
	uint8_t data8[32];/**< array for padding */
} adc_t;

/** @brief  Controls and calculates PWM values */
typedef union {
	struct {
		basic_mode_t mode; /**< basic mode for pwm settings */
		basic_gpio_t dut_pwm; /**<  */
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
		basic_gpio_t dut_dac; /**<  */
		uint16_t level; /**< The percent/100 of output level */
		uint8_t res[12]; /**< Reserved bytes */
	};
	uint8_t data8[16];/**< array for padding */
} dac_t;

/** @brief  Controls timer input capture values */
typedef union {
	struct {
		tmr_mode_t mode; /**< basic mode for dac settings */
		basic_gpio_t dut_ic; /**<  */
		uint16_t min_holdoff; /**< The minimum amount of time to wait before triggering another event in ns */
		uint32_t min_tick; /**< Minimum tick difference */
		uint32_t max_tick; /**< Maximum tick difference */
	};
	uint8_t data8[12];/**< array for padding */
} tmr_t;

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
		uint8_t tick_div[128]; /**< The tick divisor of the event - max should be 16 for interface */
		uint8_t source[128]; /**< The event source of the event - 0:no source selected - 1:DEBUG0 pin - 2:DEBUG1 pin - 3:DEBUG2 pin - 4:DUT_IC */
		uint16_t value[128]; /**< The value of the event - 0:falling edge interrupt - 1:rising edge interrupt */
		uint32_t tick[128]; /**< The tick when the event occurred */
	};
	uint8_t data8[1028];/**< array for padding */
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
		tmr_t tmr; /**< TMR configuration */
		gpio_t gpio[3]; /**< GPIO pins available */
		trace_t trace; /**< Saved timestamps and events */
		uint8_t res[216]; /**< Reserved bytes */
	};
	uint8_t data8[2048];/**< array for padding */
} map_t;

#pragma pack()
#endif /* PHILIP_TYPEDEF_H */
/** @} */
