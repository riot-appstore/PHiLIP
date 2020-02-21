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
 * @file			pwm_dac.h
 * @author			Kevin Weiss
 * @date			15.03.2019
 * @brief			Controls the pwm and dac peripheral.
 * @note			The PWM and DAC share the same timer so changing the period
 * 					affects both.
 ******************************************************************************
 */

#ifndef PWM_DAC_H_
#define PWM_DAC_H_

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes pwm and dac registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates pwm defaults registers and assigns pwm and dac
 * 				register pointers.
 */
void init_dut_pwm_dac(map_t *reg, map_t *saved_reg);

/**
 * @brief		Commits the dut pwm registers and executes operations.
 *
 * @pre			pwm must first be initialized with init_dut_pwm_dac()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the pwm.mode.init is clear.
 */
error_t commit_dut_pwm();

/**
 * @brief		Commits the dut dac registers and executes operations.
 *
 * @pre			dac must first be initialized with init_dut_pwm_dac()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 * @return      EINVAL value is too large
 *
 * @note		Only executes actions if the dac.mode.init is clear.
 */
error_t commit_dut_dac();

/**
 * @brief		Updates the pwm input levels.
 */
void update_dut_pwm_inputs();

/**
 * @brief		Updates the dac input levels.
 */
void update_dut_dac_inputs();

#endif /* PWM_DAC_H_ */
/** @} */

