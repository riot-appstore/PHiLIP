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
 * @file			uart.h
 * @author			Kevin Weiss
 * @author			Yegor Yefremov
 * @date			13.02.2019
 * @brief			Controls the uart peripherals.
 ******************************************************************************
 */

#ifndef UART_H_
#define UART_H_

/* Defines ********************************************************************/
/** @brief	Size allocated for if uart buffer */
#define UART_IF_BUF_SIZE	(1024)

/** @brief	Size allocated for dut uart buffer */
#define UART_DUT_BUF_SIZE	(128)


/* Function prototypes ********************************************************/
/**
 * @brief		Initializes dut uart registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		Populates dut uart defaults registers and assigns uart register
 * 				pointers.
 */
void init_dut_uart(map_t *reg);

/**
 * @brief		Initializes interface uart registers.
 */
void init_if_uart();

/**
 * @brief		Commits the dut uart registers and executes operations.
 *
 * @pre			uart must first be initialized with init_dut_uart()
 * @return      0 Success
 *
 * @note		Only executes actions if the uart.mode.init is set.
 */
error_t commit_dut_uart();

/**
 * @brief		Polls for any commands from the dut uart.
 *
 * @pre			uart must first be initialized with init_dut_uart()
 * @return      0 Success
 * @return 		EPROTONOSUPPORT command not supported
 * @return 		EACCES caller doesn't have access
 * @return 		EMSGSIZE message size too big
 * @return 		EINVAL Invalid value
 * @return 		EOVERFLOW invalid address
 * @return 		ERANGE invalid number range
 * @return 		ENODATA not enough data
 */
error_t poll_dut_uart();

/**
 * @brief		Polls for any commands from the interface uart.
 *
 * @pre			uart must first be initialized with init_if_uart()
 * @return      0 Success
 * @return 		EPROTONOSUPPORT command not supported
 * @return 		EACCES caller doesn't have access
 * @return 		EMSGSIZE message size too big
 * @return 		EINVAL Invalid value
 * @return 		EOVERFLOW invalid address
 * @return 		ERANGE invalid number range
 * @return 		ENODATA not enough data
 */
error_t poll_if_uart();

/**
 * @brief		Updates the uart input levels.
 */
void update_dut_uart_inputs();

/**
 * @brief This function handles dut_cts event interrupt.
 */
void dut_cts_int();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_dut_uart_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_dut_uart_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_if_uart_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_if_uart_msp();

#endif /* UART_H_ */
/** @} */
