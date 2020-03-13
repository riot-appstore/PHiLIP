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
 * 					The interrupt mode of the GPIO can log ticks at a rate of
 * 					100kHz, any faster and logs will be missed.
 *
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

	HAL_NVIC_SetPriority(GPIO_DEBUG0_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_SetPriority(GPIO_DEBUG1_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_SetPriority(GPIO_DEBUG2_IRQ, DEFAULT_INT_PRIO, 0);
	commit_debug();
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

