/*
 * MIT License
 *
 * Copyright 2018 Kevin Weiss for HAW Hamburg
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes i2c registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates i2c defaults registers and assigns i2c register
 * 				pointers.
 */
void init_dut_i2c(map_t *reg, map_t *saved_reg);

/**
 * @brief		Commits the i2c registers and executes operations.
 *
 * @pre			i2c must first be initialized with init_dut_i2c()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the i2c.mode.init is set.
 */
error_t commit_dut_i2c();

/**
 * @brief		Updates the i2c input levels.
 */
void update_dut_i2c_inputs();

#endif /* I2C_H_ */
/** @} */
