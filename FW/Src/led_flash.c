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
 * @file			led_flash.c
 * @author			Kevin Weiss
 * @date			21.02.2019
 * @brief			Flashes the LED, the most important thing in embedded.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "app_defaults.h"
#include "port.h"

/* Private enums/structs -----------------------------------------------------*/
/** @brief  		The states of flashing the led */
enum FLASH_STATE {
	FLASH_START, /**< Start flash waits longer than the normal waits */
	FLASH_MAJOR, /**< Flashes the major version + 1 */
	FLASH_WAIT_MAJOR, /**< Waits a short time to indicate then next flashes */
	FLASH_MINOR, /**< Flashes the minor version + 1 */
	FLASH_WAIT_MINOR, /**< Waits a short time to indicate then next flashes */
	FLASH_PATCH, /**< Flashes the patch version + 1 */
};


/* Private defines -----------------------------------------------------------*/
/** @brief  	How long it takes to indicate the start of led flashing */
#define START_WAIT_FLASHES	(6 * 2)

/** @brief  	How long it takes to indicate the next led flashing state */
#define WAIT_FLASHES	(2 * 2)

/** @brief  	Start flash waits longer than the normal waits */
#define MAJOR_FLASHES	((FW_REV_MAJOR + 1) * 2)

/** @brief  	Start flash waits longer than the normal waits */
#define MINOR_FLASHES	((FW_REV_MINOR + 1) * 2)

/** @brief  	Start flash waits longer than the normal waits */
#define PATCH_FLASHES	((FW_REV_PATCH + 1) * 2)

/** @brief  	How long the led takes to toggle in ms */
#define FLASH_TIME_MS	150
/** @} */

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Flashes the firmware major, minor, and patch version (+1) on
 * 				on the led.
 *
 * @pre			Must be called within a tick context, 1 ms or so.
 */
void flash_fw_version() {
	const uint8_t const amount_of_flashes[] = {	START_WAIT_FLASHES,
												MAJOR_FLASHES,
												WAIT_FLASHES,
												MINOR_FLASHES,
												WAIT_FLASHES,
												PATCH_FLASHES };
	static uint8_t state = 0;
	static uint8_t flash_tick = 0;
	static uint8_t flashes = 0;
	flash_tick++;
	if (flash_tick >= FLASH_TIME_MS) {
		flash_tick = 0;
		/* since we dont want to flash during all wait periods, and they are
		 * all even numbers we take advantage.  Though writing this makes it
		 * a bit pointless...
		 */
		if (state & 0x1) {
			HAL_GPIO_TogglePin(LED0);
		}

		if (flashes >= amount_of_flashes[state]) {
			flashes = 0;
			state++;
			if (state >= sizeof(amount_of_flashes)) {
				state = 0;
			}
		}
		flashes++;
	}
}
