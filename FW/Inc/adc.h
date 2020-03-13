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
 * @file			adc.h
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Controls the adc peripheral.
 ******************************************************************************
 */
#ifndef ADC_H_
#define ADC_H_

/* Function prototypes ********************************************************/
/**
 * @brief		Initializes adc registers and peripheral.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		Instantiates the dut_adc module device.
 */
void init_dut_adc(map_t *reg);

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_dut_adc_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_dut_adc_msp();

/**
 * @brief		Commits the adc registers and executes operations.
 *
 * @pre			adc must first be initialized with init_adc()
 * @return      0 		success
 * @return      -EINVAL	invalid value
 *
 * @note		Only executes actions if the adc.mode.init == 0.
 */
error_t commit_dut_adc();

/**
 * @brief		Updated the map with ADC values.
 *
 * @note		Since the adc_t does not interact with any interrupts
 * 				protections are not needed or used.
 */
void poll_dut_adc();

#endif /* ADC_H_ */
/** @} */
