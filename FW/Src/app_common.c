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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "app_common.h"

/* Private function prototypes -----------------------------------------------*/
static void _delay_ticks(uint64_t delay);

/* Functions -----------------------------------------------------------------*/
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

/**
 * @brief  		Imprecise blocking delay in microseconds.
 *
 * @param[in]   micros  Number of microseconds to delay
 */
void delay_us(uint16_t micros) {
	uint64_t freq = HAL_RCC_GetSysClockFreq() / 1000000;
	freq *= micros;
	_delay_ticks(freq);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
/**
 * @brief		Copies and compares data until copied data matches.
 *
 * @param[out]	dest	Destination address to copy to
 * @param[in]	src		Source address to copy
 * @param[in]	size	Size of bytes to copy
 * @note		Used for data concurrency issues without blocking interrupts.
 */
void copy_until_same(void *dest, void *src, size_t size) {

	while (memcmp(dest, src, size)) {
		memcpy(dest, src, size);
	}
}
#pragma GCC pop_options

/** @brief	Wrapper function to disable interrupts. */
inline void disable_interrupt() {
	__disable_irq();
}

/** @brief	Wrapper function to enable interrupts. */
inline void enable_interrupt() {
	__enable_irq();
}

/** @brief	Wrapper function to provide software reset. */
inline void soft_reset() {
	NVIC_SystemReset();
}

/** @brief	Gets tick value in amount to system tick (72 MHz). */
uint64_t get_tick() {
	uint32_t sys_tick;
	uint32_t tick;
	do {
		sys_tick = SysTick->VAL;
		tick = HAL_GetTick();
	} while (SysTick->VAL > sys_tick);
	return (tick << TICK_BIT_OFFSET) + (SysTick->LOAD - sys_tick);
}

/** @brief	Gets tick value in amount to system tick (72 MHz) in 32bit. */
uint32_t get_tick32() {
	uint32_t sys_tick;
	uint32_t tick;
	do {
		sys_tick = SysTick->VAL;
		tick = HAL_GetTick();
	} while (SysTick->VAL > sys_tick);
	return (uint32_t) (((uint32_t) tick << TICK_BIT_OFFSET)
			+ (SysTick->LOAD - sys_tick));
}

/**
 * @brief		Gets 32 bit tick shifted down by an amount.
 *
 * @param[in]	div		Divisor, divide ticks by 2^n
 */
uint32_t get_tick32_div(uint8_t div) {
	uint32_t sys_tick;
	uint32_t tick;
	do {
		sys_tick = SysTick->VAL;
		tick = HAL_GetTick();
	} while (SysTick->VAL > sys_tick);
	return (uint32_t) (((uint32_t) (tick << (TICK_BIT_OFFSET - div))
			+ ((SysTick->LOAD - sys_tick) >> div)));
}
/** @} */
