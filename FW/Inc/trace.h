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
 * @file			trace.h
 * @author			Kevin Weiss
 * @date			12.03.2019
 * @brief			Used for logging traces.
 ******************************************************************************
 */
#ifndef TRACE_H_
#define TRACE_H_

/* Defines -------------------------------------------------------------------*/
/** @brief  	The sources of an event. */
enum EVENT_SOURCES {
	SOURCE_NONE, /**< No source selected, should be invalid */
	SOURCE_DEBUG0, /**< Source is from DEBUG0 pin */
	SOURCE_DEBUG1, /**< Source is from DEBUG1 pin */
	SOURCE_DEBUG2 /**< Source is from DEBUG2 pin */
};

/** @brief  	Type of trigger causing the event. */
enum EVENT_TYPE {
	EVENT_TYPE_FALLING, /**< Falling edge interrupt */
	EVENT_TYPE_RISING /**< Rising edge interrupt */
};


/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes trace register.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		No saved reg is needed since nothing is configurable
 */
void init_trace(map_t *reg);
/**
 * @brief		Stores an gpio toggle event.
 *
 * @param[in]	tick_div	Divides ticks by power of 2
 * @param[in]	source		The source of the event
 * @param[in]	value		The value from the event
 */
void store_gpio_trace(uint8_t tick_div, uint8_t source, uint16_t value);

#endif /* TRACE_H_ */
/** @} */
