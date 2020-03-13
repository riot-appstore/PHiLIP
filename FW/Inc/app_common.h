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
 * @file			app_common.h
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			For common application functions.
 * @details			Deals with simple delays and interrupt control, mostly
 *                  wrappers around hardware specific functions.
 ******************************************************************************
 */

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

/* Defines ********************************************************************/
/** @brief	Disables interrupts. */
#define DIS_INT __disable_irq();

/** @brief	Enables interrupts. */
#define EN_INT __enable_irq();

/**
 * @brief	Resets device.
 * @note	This is a software reset, if the reset pin is being held the
 * 			device may not complete reset until released.
 */
#define SOFT_RESET	soft_reset()

/** @brief	Bit shifting ticks. */
#define TICK_BIT_OFFSET 16

/* Function prototypes ********************************************************/
/**
 * @brief  		Imprecise blocking delay in microseconds.
 *
 * @param[in]   micros  Number of microseconds to delay
 */
void delay_us(uint16_t micros);

/**
 * @brief		Copies and compares data until copied data matches.
 *
 * @param[out]	dest	Destination address to copy to
 * @param[in]	src		Source address to copy
 * @param[in]	size	Size of bytes to copy
 * @note		Used for data concurrency issues without blocking interrupts.
 */
void copy_until_same(void *dest, void *src, size_t size);

/** @brief	Wrapper function to provide software reset. */
void soft_reset();

/** @brief	Gets tick value in amount to system tick (72 MHz). */
uint64_t get_tick();

/** @brief	Gets tick value in amount to system tick (72 MHz) in 32bit. */
uint32_t get_tick32();

/**
 * @brief		Gets 32 bit tick shifted down by an amount.
 *
 * @param[in]	div		Divisor, divide ticks by 2^n
 */
uint32_t get_tick32_div(uint8_t div);

#endif /* APP_COMMON_H_ */
/** @} */
