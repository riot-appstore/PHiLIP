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
 * @file			uart.h
 * @author			Kevin Weiss
 * @author			Yegor Yefremov
 * @date			13.02.2019
 * @brief			Controls the uart peripherals.
 ******************************************************************************
 */

#ifndef UART_H_
#define UART_H_

/* Defines -------------------------------------------------------------------*/
/** @brief	Size allocated for uart buffers */
#define UART_BUF_SIZE	(256 + 128)

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes dut uart registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates dut uart defaults registers and assigns uart register
 * 				pointers.
 */
void init_dut_uart(map_t *reg, map_t *saved_reg);

/**
 * @brief		Initializes interface uart registers.
 *
 * @note		Populates dut uart defaults registers, uart register are NULL.
 */
void init_if_uart();

/**
 * @brief		Commits the dut uart registers and executes operations.
 *
 * @pre			uart must first be initialized with init_dut_uart()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the uart.mode.init is set.
 */
error_t commit_dut_uart();

/**
 * @brief		Polls for any commands from the dut uart.
 *
 * @pre			uart must first be initialized with init_dut_uart()
 * @return      EOK on success
 * @return      ENOACTION no action occurred
 * @return 		EPROTONOSUPPORT command not supported
 * @return 		EACCES caller doesn't have access
 * @return 		EMSGSIZE message size too big
 * @return 		EINVAL Invalid value
 * @return 		EOVERFLOW invalid address
 * @return 		ERANGE invalid number range
 * @return 		ENODATA not enough data
 * @return 		EUNKNOWN
 */
error_t poll_dut_uart();

/**
 * @brief		Polls for any commands from the interface uart.
 *
 * @pre			uart must first be initialized with init_if_uart()
 * @return      EOK on success
 * @return      ENOACTION no action occurred
 * @return 		EPROTONOSUPPORT command not supported
 * @return 		EACCES caller doesn't have access
 * @return 		EMSGSIZE message size too big
 * @return 		EINVAL Invalid value
 * @return 		EOVERFLOW invalid address
 * @return 		ERANGE invalid number range
 * @return 		ENODATA not enough data
 * @return 		EUNKNOWN
 */
error_t poll_if_uart();

/**
 * @brief This function handles dut_cts event interrupt.
 */
void dut_cts_int();

#endif /* UART_H_ */
/** @} */
