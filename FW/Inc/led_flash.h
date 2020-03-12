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
 * @file			led_flash.h
 * @author			Kevin Weiss
 * @date			21.02.2019
 * @brief			Flashes the LED, the most important thing in embedded.
 ******************************************************************************
 */

#ifndef LED_FLASH_H_
#define LED_FLASH_H_

/* Function prototypes ********************************************************/
/**
 * @brief		Initialize LED0 gpio.
 */
void init_led_flash();

/**
 * @brief		Flashes the firmware major, minor, and patch version (+1) on
 * 				on the led.
 *
 * @pre			Must be called within a tick context, 1 ms or so.
 */
void flash_fw_version();

#endif /* LED_FLASH_H_ */
/** @} */
