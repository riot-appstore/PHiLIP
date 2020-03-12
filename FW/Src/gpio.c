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
 * @file			gpio.c
 * @author			Kevin Weiss
 * @date			18.02.2019
 * @brief			GPIO management.
 * @details			Controls all basic GPIO functions such as interrupts and
 * 					initialization.
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "app_common.h"
#include "port.h"
#include "trace.h"
#include "uart.h"
#include "spi.h"

#include "gpio.h"

/* Private enums/structs ******************************************************/
/** @brief  			enum for the type of gpio setting. */
enum GPIO_IO_TYPE {
	GPIO_IN, /**< gpio is high impedance input */
	GPIO_OUT_PP, /**< gpio is push pull output */
	GPIO_OUT_OD, /**< gpio is open drain output */
	GPIO_INTERRUPT /**< gpio interrupts and saves event */
};

/** @brief				Parameters for gpio control. */
typedef struct {
	GPIO_InitTypeDef hgpio; /**< handle for HAL gpio */
	gpio_t *reg; /**< gpio application registers */
	gpio_mode_t mode; /**< gpio mode */
	GPIO_TypeDef *port; /**< port of gpio */
} gpio_dev;
/** @} */

/* Private function prototypes ************************************************/
static error_t _commit_gpio(gpio_dev *dev);

/* Private variables **********************************************************/
static gpio_dev debug_gpio[3];

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_gpio(map_t *reg) {
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(TEST_WARN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TEST_FAIL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TEST_PASS, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED0, LED_OFF);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DUT_RST, GPIO_PIN_SET);

	/*Configure GPIO pin : USER_BTN_Pin */
	GPIO_InitStruct.Pin = USER_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TEST_WARN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(TEST_WARN_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TEST_FAIL_Pin;
	HAL_GPIO_Init(TEST_FAIL_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TEST_PASS_Pin;
	HAL_GPIO_Init(TEST_PASS_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED0_Pin;
	HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_RTS_Pin;
	HAL_GPIO_Init(DUT_RTS_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DUT_RST_GPIO_Port, &GPIO_InitStruct);

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


	HAL_NVIC_SetPriority(GPIO_NSS_CTS_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_SetPriority(GPIO_DEBUG0_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_SetPriority(GPIO_DEBUG1_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_SetPriority(GPIO_DEBUG2_IRQ, DEFAULT_INT_PRIO, 0);

	debug_gpio[0].port = DEBUG0_GPIO_Port;
	debug_gpio[0].hgpio.Pin = DEBUG0_Pin;
	debug_gpio[0].hgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	debug_gpio[0].reg = &(reg->gpio[0]);

	debug_gpio[1].port = DEBUG1_GPIO_Port;
	debug_gpio[1].hgpio.Pin = DEBUG1_Pin;
	debug_gpio[1].hgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	debug_gpio[1].reg = &(reg->gpio[1]);

	debug_gpio[2].port = DEBUG2_GPIO_Port;
	debug_gpio[2].hgpio.Pin = DEBUG2_Pin;
	debug_gpio[2].hgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	debug_gpio[2].reg = &(reg->gpio[2]);

	commit_debug();
	HAL_NVIC_EnableIRQ(GPIO_NSS_CTS_IRQ);

}

/******************************************************************************/
error_t commit_debug() {
	error_t err = 0;
	HAL_NVIC_DisableIRQ(GPIO_DEBUG0_IRQ);
	HAL_NVIC_DisableIRQ(GPIO_DEBUG1_IRQ);
	HAL_NVIC_DisableIRQ(GPIO_DEBUG2_IRQ);

	err = _commit_gpio(&debug_gpio[0]);
	if (err != 0) {
		return err;
	}
	err = _commit_gpio(&debug_gpio[1]);
	if (err != 0) {
		return err;
	}
	err = _commit_gpio(&debug_gpio[2]);
	EXTI->PR = DEBUG0_Pin | DEBUG1_Pin | DEBUG2_Pin;
	HAL_NVIC_EnableIRQ(GPIO_DEBUG0_IRQ);
	HAL_NVIC_EnableIRQ(GPIO_DEBUG1_IRQ);
	HAL_NVIC_EnableIRQ(GPIO_DEBUG2_IRQ);
	return err;
}

static error_t _commit_gpio(gpio_dev *dev) {
	if (dev->reg->mode.init) {
		return 0;
	}
	if (dev->reg->mode.pull > GPIO_PULLDOWN) {
		return EINVAL;
	}
	if (dev->reg->mode.io_type == GPIO_OUT_PP) {
		HAL_GPIO_WritePin(dev->port, dev->hgpio.Pin, dev->reg->mode.level);
		dev->hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	} else if (dev->reg->mode.io_type == GPIO_OUT_OD) {
		HAL_GPIO_WritePin(dev->port, dev->hgpio.Pin, dev->reg->mode.level);
		dev->hgpio.Mode = GPIO_MODE_OUTPUT_OD;
	} else if (dev->reg->mode.io_type == GPIO_IN) {
		dev->hgpio.Mode = GPIO_MODE_INPUT;
		dev->hgpio.Pull = dev->reg->mode.pull;
	} else if (dev->reg->mode.io_type == GPIO_INTERRUPT) {
		dev->hgpio.Mode = GPIO_MODE_IT_RISING_FALLING;
		dev->hgpio.Pull = dev->reg->mode.pull;
	} else {
		return EINVAL;
	}
	dev->mode.io_type = dev->reg->mode.io_type;
	dev->mode.tick_div = dev->reg->mode.tick_div;

	dev->reg->mode.init = 1;

	HAL_GPIO_Init(dev->port, &dev->hgpio);
	return 0;
}

/******************************************************************************/
error_t init_basic_gpio(basic_gpio_t gpio, GPIO_TypeDef *port, uint32_t pin) {
	GPIO_InitTypeDef hgpio = {0};
	hgpio.Speed = GPIO_SPEED_FREQ_LOW;
	hgpio.Pin = pin;
	if (gpio.pull > GPIO_PULLDOWN) {
		return EINVAL;
	}
	if (gpio.io_type == GPIO_OUT_PP) {
		HAL_GPIO_WritePin(port, pin, gpio.set_level);
		hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	} else if (gpio.io_type == GPIO_OUT_OD) {
		HAL_GPIO_WritePin(port, pin, gpio.set_level);
		hgpio.Mode = GPIO_MODE_OUTPUT_OD;
	} else if (gpio.io_type == GPIO_IN) {
		hgpio.Mode = GPIO_MODE_INPUT;
		hgpio.Pull = gpio.pull;
	} else {
		return EINVAL;
	}
	HAL_GPIO_Init(port, &hgpio);
	return 0;
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void update_debug_inputs() {
	for (int i = 0; i < 3; i++) {
		if (debug_gpio[i].mode.io_type == GPIO_IN) {
			debug_gpio[i].reg->status.level =
				HAL_GPIO_ReadPin(debug_gpio[i].port,
							     debug_gpio[i].hgpio.Pin);
		}
	}
}
/******************************************************************************/
/*           Interrupt Handling                                               */
/******************************************************************************/
/**
 * @brief 	Interrupt for NSS and CTS triggering
 * @note 	This is a shared interrupt, both functions being used at the same
 * 			time should be avoided.
 */
void GPIO_NSS_CTS_INT() {
	uint32_t int_pins = EXTI->PR;
	if (DUT_NSS_Pin & int_pins) {
		dut_nss_int();
		EXTI->PR = DUT_NSS_Pin | DUT_CTS_Pin;
		/* Exit quickly since spi response must be ASAP */
		return;
	} else if (DUT_CTS_Pin & int_pins) {
		dut_cts_int();
	}
	/* clear interrupts */
	EXTI->PR = int_pins;
}

/**
 * @brief 	Interrupt for the DEBUG0 triggering when interrupt mode set
 */
void GPIO_DEBUG0_INT() {
	store_gpio_trace(debug_gpio[0].mode.tick_div, SOURCE_DEBUG0,
			HAL_GPIO_ReadPin(DEBUG0));
	EXTI->PR = DEBUG0_Pin;
}

/**
 * @brief 	Interrupt for the DEBUG1 triggering when interrupt mode set
 */
void GPIO_DEBUG1_INT() {
	store_gpio_trace(debug_gpio[1].mode.tick_div, SOURCE_DEBUG1,
			HAL_GPIO_ReadPin(DEBUG1));
	EXTI->PR = DEBUG1_Pin;
}

/**
 * @brief 	Interrupt for the DEBUG2 triggering when interrupt mode set
 */
void GPIO_DEBUG2_INT() {
	store_gpio_trace(debug_gpio[2].mode.tick_div, SOURCE_DEBUG2,
			HAL_GPIO_ReadPin(DEBUG2));
	EXTI->PR = DEBUG2_Pin;
}

