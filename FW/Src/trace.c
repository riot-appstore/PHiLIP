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

/**
 * @brief		Stores an gpio toggle event.
 *
 * @param[in]	tick_div	Divides ticks by power of 2
 * @param[in]	source		The source of the event
 * @param[in]	value		The value from the event
 */
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
