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
 * @file			gpio.c
 * @author			Kevin Weiss
 * @date			18.02.2019
 * @brief			GPIO management.
 * @details			Controls all basic GPIO functions such as interrupts and
 * 					initialization.
 * @}
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "uart.h"
#include "spi.h"

#include "gpio.h"

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes gpio directions, states and interrupt settings.
 */
void init_gpio() {
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
	TEST_WARN_Pin | TEST_FAIL_Pin | TEST_PASS_Pin | LED0_Pin | DUT_RTS_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, DEBUG0_Pin | DEBUG1_Pin | DEBUG2_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin : USER_BTN_Pin */
	GPIO_InitStruct.Pin = USER_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : TEST_WARN_Pin TEST_FAIL_Pin TEST_PASS_Pin LED0_Pin
	 DUT_RTS_Pin */
	GPIO_InitStruct.Pin = TEST_WARN_Pin | TEST_FAIL_Pin | TEST_PASS_Pin
			| LED0_Pin | DUT_RTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : DUT_RST_Pin DEBUG0_Pin DEBUG1_Pin DEBUG2_Pin */
	GPIO_InitStruct.Pin = DUT_RST_Pin | DEBUG0_Pin | DEBUG1_Pin | DEBUG2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : DUT_CTS_Pin */
	GPIO_InitStruct.Pin = DUT_CTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(DUT_CTS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DUT_NSS_Pin */
	GPIO_InitStruct.Pin = DUT_NSS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DUT_NSS_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief Interrupt for a GPIO triggering
 */
void GPIO_INT() {
	uint32_t int_pins = EXTI->PR;
	if (DUT_NSS_Pin & int_pins) {
		dut_nss_int();
	} else if (DUT_CTS_Pin & int_pins) {
		dut_cts_int();
	}
	/* clear interrupts */
	EXTI->PR = int_pins;
}
