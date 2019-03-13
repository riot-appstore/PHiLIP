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

/* Macros --------------------------------------------------------------------*/
/** @brief	Disables interrupts. */
#define DIS_INT disable_interrupt()

/** @brief	Enables interrupts. */
#define EN_INT enable_interrupt()

/**
 * @brief	Resets device.
 * @note	This is a software reset, if the reset pin is being held the
 * 			device may not complete reset until released.
 */
#define SOFT_RESET	soft_reset()

/** @brief	Bit shifting ticks. */
#define TICK_BIT_OFFSET 16

/* Function prototypes -------------------------------------------------------*/
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

/** @brief	Wrapper function to disable interrupts. */
void disable_interrupt();

/** @brief	Wrapper function to enable interrupts. */
void enable_interrupt();

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
