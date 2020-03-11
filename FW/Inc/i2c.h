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
 * @file			i2c.h
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Controls the i2c peripheral.
 ******************************************************************************
 */

#ifndef I2C_H_
#define I2C_H_

/* Function prototypes ********************************************************/
/**
 * @brief		Initializes i2c registers.
 *
 * @param[in]	reg			Pointer to register memory map
 * @note		Populates i2c defaults registers and assigns i2c register
 * 				pointers.
 */
void init_dut_i2c(map_t *reg);

/**
 * @brief		Commits the i2c registers and executes operations.
 *
 * @pre			i2c must first be initialized with init_dut_i2c()
 * @return      0 if OK
 *
 * @note		Only executes actions if the i2c.mode.init is clear.
 */
error_t commit_dut_i2c();

/**
 * @brief		Updates the i2c input levels.
 */
void update_dut_i2c_inputs();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_dut_i2c_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_dut_i2c_msp();

#endif /* I2C_H_ */
/** @} */
