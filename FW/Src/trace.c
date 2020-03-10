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
 * @file			trace.c
 * @author			Kevin Weiss
 * @date			12.03.2019
 * @brief			Used for logging traces.
 * @}
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>

#include "PHiLIP_typedef.h"
#include "app_common.h"

#include "trace.h"

/* Private variables ---------------------------------------------------------*/
/** @brief		Gets the buffer size of the number of traces. */
#define NUM_OF_TRACES	(sizeof(((map_t*)0)->trace.tick)/sizeof(((map_t*)0)->trace.tick[0]))

/* Private variables ---------------------------------------------------------*/
static trace_t *trace;

/**
 * @brief		Initializes trace register.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		No saved reg is needed since nothing is configurable
 */
void init_trace(map_t *reg) {
	trace = &(reg->trace);
}

void store_gpio_trace(uint8_t tick_div, uint8_t source, uint16_t value) {
	trace->source[trace->index] = source;
	trace->tick_div[trace->index] = tick_div;
	trace->value[trace->index] = value;
	trace->tick[trace->index] = get_tick32_div(tick_div);
	trace->index++;
	if (trace->index >= NUM_OF_TRACES) {
		trace->index = 0;
	}
}

void store_tick_from_buf(uint8_t source, uint16_t value, uint32_t tick) {
	trace->source[trace->index] = source;
	trace->tick_div[trace->index] = 0;
	trace->value[trace->index] = value;
	trace->tick[trace->index] = tick;
	trace->index++;
	if (trace->index >= NUM_OF_TRACES) {
		trace->index = 0;
	}
}
