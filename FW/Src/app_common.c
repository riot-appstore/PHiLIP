/*
 * app_common.c
 *
 *  Created on: May 18, 2018
 *      Author: kevinweiss
 */

#include "stm32f1xx_hal.h"

#include "app_common.h"

void _delay_ticks(uint64_t delay) {

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

inline void disable_interrupt() {
	__disable_irq();
}

inline void enable_interrupt() {
	__enable_irq();
}

inline void soft_reset() {
	NVIC_SystemReset();
}
