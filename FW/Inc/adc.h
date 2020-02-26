/*
 * MIT License
 *
 * Copyright 2019 Kevin Weiss for HAW Hamburg
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
 * @file			adc.h
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Controls the adc peripheral.
 ******************************************************************************
 */
#ifndef ADC_H_
#define ADC_H_

/* Function prototypes -------------------------------------------------------*/
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
