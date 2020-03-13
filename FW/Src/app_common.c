/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @addtogroup Application
 * @{
 * @file			app_common.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			For common application functions.
 * @details			Deals with simple delays and interrupt control, mostly
 *                  wrappers around hardware specific functions.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <string.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "app_common.h"

/* Private function prototypes ************************************************/
static void _delay_ticks(uint64_t delay);

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
static void _delay_ticks(uint64_t delay) {
	uint64_t ticks = 0;
	int32_t old_tick = SysTick->VAL;
	int32_t new_tick;
	int32_t diff;

	do {
		new_tick = SysTick->VAL;
		diff = old_tick - new_tick;
		if (diff < 0) {
			diff += SysTick->LOAD;
		}
		ticks += diff;
		old_tick = new_tick;
	} while (ticks < delay);
}

void delay_us(uint16_t micros) {
	uint64_t freq = HAL_RCC_GetSysClockFreq() / 1000000;
	freq *= micros;
	_delay_ticks(freq);
}

uint64_t get_tick() {
	uint32_t sys_tick;
	uint32_t tick;
	do {
		sys_tick = SysTick->VAL;
		tick = HAL_GetTick();
	} while (SysTick->VAL > sys_tick);
	return ((uint64_t)tick << TICK_BIT_OFFSET) + (SysTick->LOAD - sys_tick);
}

uint32_t get_tick32() {
	uint32_t sys_tick;
	uint32_t tick;
	do {
		sys_tick = SysTick->VAL;
		tick = HAL_GetTick();
	} while (SysTick->VAL > sys_tick);
	return (( tick << TICK_BIT_OFFSET) + (SysTick->LOAD - sys_tick));
}


uint32_t get_tick32_div(uint8_t div) {
	uint32_t sys_tick;
	uint32_t tick;
	int runs = 0;
	do {
		runs++;
		sys_tick = SysTick->VAL;
		tick = HAL_GetTick();
	} while (SysTick->VAL > sys_tick);
	if (runs > 1) {
		/* Overflow occurred */
		/* Since this is the same priority as the systick interrupt we know
		 * an increment should have occurred but didn't.
		 */
		tick++;
	}
	return (uint32_t) (((uint32_t) (tick << (TICK_BIT_OFFSET - div))
			+ ((SysTick->LOAD - sys_tick) >> div)));
}
/******************************************************************************/
#pragma GCC push_options
#pragma GCC optimize ("O0")
void copy_until_same(void *dest, void *src, size_t size) {

	while (memcmp(dest, src, size)) {
		memcpy(dest, src, size);
	}
}
#pragma GCC pop_options

/******************************************************************************/
inline void soft_reset() {
	NVIC_SystemReset();
}
