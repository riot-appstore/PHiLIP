/*
 * Filename: bpt_mem_map.g
 * Author: Kevin Weiss
 * Revision: 1.00.02
 */

#ifndef BPT_MEM_MAP_H_
#define BPT_MEM_MAP_H_

#include <stdint.h>

#pragma pack(1)
/* @brief Time and date */
typedef union timestamp_t_TAG {
	struct {
		/* The seconds in decimal */
		uint8_t second;
		/* The minutes in decimal */
		uint8_t minute;
		/* The hours in decimal */
		uint8_t hour;
		/* The months in decimal */
		uint8_t day_of_month;
		/* The week in decimal */
		uint8_t day_of_week;
		/* The month in decimal */
		uint8_t month;
		/* The last two digits of the year in decimal (20xx) */
		uint8_t year;
		/* Reserved bytes */
		uint8_t res[1];
	};
	uint8_t data8[8];
} timestamp_t;

/* @brief Control register for device */
typedef struct sys_cr_t_TAG {
	/* resets the DUT */
	uint8_t dut_rst : 1;
} sys_cr_t;

/* @brief System settings for the bpt */
typedef union sys_t_TAG {
	struct {
		/* Unique ID of the device */
		uint8_t sn[12];
		/* Firmware revision */
		uint32_t fw_rev;
		/* time of build */
		timestamp_t build_time;
		/* A constant number that should always be the same */
		uint32_t device_num;
		/* Control register for device */
		sys_cr_t cr;
		/* Reserved bytes */
		uint8_t res[3];
	};
	uint8_t data8[32];
} sys_t;

/* @brief Specific modes for I2C */
typedef struct i2c_mode_t_TAG {
	/* 10 bit address enable */
	uint8_t addr_10_bit : 1;
	/* General call enable */
	uint8_t general_call : 1;
	/* Disable for clk stretch */
	uint8_t no_clk_stretch : 1;
	/* 16 bit register access mode */
	uint8_t reg_16_bit : 1;
	/* Forces a data nack */
	uint8_t nack_data : 1;
} i2c_mode_t;

/* @brief Specific modes for I2C */
typedef struct i2c_status_t_TAG {
	/* Overrun/Underrun: Request for new byte when not ready */
	uint8_t OVR : 1;
	/* Acknowledge failure */
	uint8_t AF : 1;
	/* Bus error:  Non-valid position during a byte transfer */
	uint8_t BERR : 1;
	/* General call address recieved */
	uint8_t GENCALL : 1;
	/* Forces a data nack */
	uint8_t BUSY : 1;
	/* Repeated start detected */
	uint8_t RSR : 1;
} i2c_status_t;

/* @brief System settings for the device */
typedef union i2c_t_TAG {
	struct {
		/* Specific modes for I2C */
		i2c_mode_t mode;
		/* Specific modes for I2C */
		i2c_status_t status;
		/* Delay in us for clock stretch */
		uint16_t clk_stretch_delay;
		/* Primary slave address */
		uint16_t slave_addr_1;
		/* Secondary slave address */
		uint16_t slave_addr_2;
		/* Last read frame byte count */
		uint8_t r_count;
		/* Last write frame byte count */
		uint8_t w_count;
		/* Reserved bytes */
		uint8_t res[6];
	};
	uint8_t data8[16];
} i2c_t;

/* @brief  */
typedef union spi_t_TAG {
	struct {
		/*  */
		uint8_t mode;
		/*  */
		uint32_t error_code;
		/* Reserved bytes */
		uint8_t res[11];
	};
	uint8_t data8[16];
} spi_t;

/* @brief UART control register */
typedef struct uart_ctrl_t_TAG {
	/* Number of stop bits */
	uint8_t stop_bits : 1;
	/* Parity */
	uint8_t parity : 2;
	/* RTS pin state */
	uint8_t rts : 1;
} uart_ctrl_t;

/* @brief UART status register */
typedef struct uart_status_t_TAG {
	/* CTS pin state */
	uint8_t cts : 1;
} uart_status_t;

/* @brief  */
typedef union uart_t_TAG {
	struct {
		/* Test mode */
		uint8_t mode;
		/* Baudrate */
		uint32_t baud;
		/* Number of received bytes */
		uint16_t rx_count;
		/* Number of transmitted bytes */
		uint16_t tx_count;
		/* UART control register */
		uart_ctrl_t ctrl;
		/* UART status register */
		uart_status_t status;
		/* Reserved bytes */
		uint8_t res[5];
	};
	uint8_t data8[16];
} uart_t;

/* @brief  */
typedef union adc_t_TAG {
	struct {
		/*  */
		uint8_t mode;
		/*  */
		uint16_t error_code;
		/*  */
		uint8_t sample_rate;
		/*  */
		uint32_t value;
		/* Reserved bytes */
		uint8_t res[8];
	};
	uint8_t data8[16];
} adc_t;

/* @brief  */
typedef union pwm_t_TAG {
	struct {
		/*  */
		uint8_t mode;
		/*  */
		uint16_t error_code;
		/*  */
		uint8_t duty;
		/*  */
		uint32_t freq;
		/* Reserved bytes */
		uint8_t res[8];
	};
	uint8_t data8[16];
} pwm_t;

/* @brief  */
typedef union tmr_t_TAG {
	struct {
		/*  */
		uint8_t mode;
		/*  */
		uint16_t error_code;
		/*  */
		uint8_t duty;
		/*  */
		uint32_t freq;
		/*  */
		uint32_t hi_us;
		/*  */
		uint32_t lo_us;
	};
	uint8_t data8[16];
} tmr_t;

/* @brief The memory map */
typedef union map_t_TAG {
	struct {
		/* system configuration (protected) */
		sys_t sys;
		/* I2C configuration */
		i2c_t i2c;
		/*  */
		spi_t spi;
		/*  */
		uart_t uart;
		/*  */
		timestamp_t rtc;
		/*  */
		adc_t adc[2];
		/*  */
		pwm_t pwm;
		/*  */
		tmr_t tmr;
		/* Writable registers for user testing */
		uint8_t user_reg[64];
		/* Reserved bytes */
		uint8_t res[40];
	};
	uint8_t data8[256];
} map_t;

#pragma pack()
#endif