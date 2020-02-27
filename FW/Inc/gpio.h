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
 * @file			gpio.h
 * @author			Kevin Weiss
 * @date			18.02.2019
 * @brief			GPIO management.
 * @details			Controls all basic GPIO functions such as interrupts and
 * 					initialization.
 ******************************************************************************
 */

#ifndef GPIO_H_
#define GPIO_H_



/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes gpio directions, states and interrupt settings.
 */
void init_gpio(map_t *reg, map_t *saved_reg);

/**
 * @brief		Commits the gpio registers and executes operations.
 *
 * @pre			gpio must first be initialized with init_gpio()
 * @return      EOK if init occurred
 * @return      EINVAL if invalid setting
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the spi.mode.init is set.
 */
error_t commit_debug();

/**
 * @brief		Updates the DEBUG input levels.
 */
void update_debug_inputs();


/**
 * @brief		Initializes a basic gpio.
 *
 * @param[in]	gpio	Parameters of GPIO such as input with pullup
 * @param[in]	port	...GPIO port
 * @param[in]	pin		...GPIO pin
 *
 * @return      EOK if init occurred
 * @return      EINVAL if invalid setting
 */
error_t init_basic_gpio(basic_gpio_t gpio,
		GPIO_TypeDef *port, uint32_t pin);

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief 	Interrupt for NSS and CTS triggering
 * @note 	This is a shared interrupt, both functions being used at the same
 * 			time should be avoided.
 */
void GPIO_NSS_CTS_INT();

/**
 * @brief 	Interrupt for the DEBUG0 triggering when interrupt mode set
 */
void GPIO_DEBUG0_INT();

/**
 * @brief 	Interrupt for the DEBUG1 triggering when interrupt mode set
 */
void GPIO_DEBUG1_INT();

/**
 * @brief 	Interrupt for the DEBUG2 triggering when interrupt mode set
 */
void GPIO_DEBUG2_INT();
#endif /* GPIO_H_ */
/** @} */
