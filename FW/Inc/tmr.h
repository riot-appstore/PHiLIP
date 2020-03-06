/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @addtogroup periph
 * @{
 * @file			tmr.c
 * @author			Kevin Weiss
 * @date			03.03.2020
 * @brief			Controls the timer peripheral.
 ******************************************************************************
 */
#ifndef TMR_H_
#define TMR_H_
/**
 * @brief		Edge select for DUT_IC.
 */
enum DUT_IC_EDGE {
	DUT_IC_EDGE_BOTH, /**< Interrupt on both edges */
	DUT_IC_EDGE_RISING, /**< Interrupt on rising edge using DMA */
	DUT_IC_EDGE_FALLING, /**< Interrupt on falling edge using DMA */
};

#define DMA_HOLDOFF_NS		200	/**< Time it takes for a DMA to load value */
#define INT_HOLDOFF_NS		1000 /**< Time it takes for a interrupt to load value */

#define OVERFLOW_TICK_VAL	0x10000 /**< 16 bit timer will overflow to this value */
/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes timer registers and peripheral.
 *
 * @param[in]	reg			Pointer to register memory map
 * @note		Instantiates the dut_i2c module device.
 */
void init_dut_ic(map_t *reg);

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_dut_ic_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_dut_ic_msp();

/**
 * @brief		Commits the adc registers and executes operations.
 *
 * @return      0 		success
 * @return      -EINVAL	invalid value
 *
 * @note		Only executes actions if the tmr.mode.init == 0.
 */
error_t commit_dut_ic();

/**
 * @brief		Polls for high speed input capture values.
 *
 * Since values must be transferred before the buffer overflows, either in DMA
 * or INT mode this must be called at a rate faster that buffer overflow can
 * occur.
 */
void poll_dut_ic();

/**
 * @brief		Updates the dut_ic input level.
 */
void update_dut_ic_inputs();

#endif /* TMR_H_ */
/** @} */
