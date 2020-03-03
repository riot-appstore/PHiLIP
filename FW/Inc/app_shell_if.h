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
 * @file			app_shell_if.h
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Protocol for application communication.
 * @details			This initializes and runs the serial communication
 * 					protocol for interfacing to registers.  It used the STM HAL
 * 					and a UART for the IO.  It also uses DMA.
 ******************************************************************************
 */

#ifndef APP_SHELL_IF_H_
#define APP_SHELL_IF_H_

/* Defines -------------------------------------------------------------------*/
/** @brief   Parse on newline */
#define RX_END_CHAR		'\n'

/** @brief   Send newline at end of transmission */
#define TX_END_STR		"\n"

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief Parses a string and executes commands.
 *
 * @param[in]	str			String with the command
 * @param[in]	buf_size	The max size of the string buffer
 * @param[in]	access		The callers access level
 *
 * @return 		EOK on success
 * @return 		EPROTONOSUPPORT command not supported
 * @return 		EACCES caller doesn't have access
 * @return 		EMSGSIZE message size too big
 * @return 		EINVAL Invalid value
 * @return 		EOVERFLOW invalid address
 * @return 		ERANGE invalid number range
 * @return 		ENODATA not enough data
 * @return 		EUNKNOWN
 *
 * @warning		May protect interrupts and cause jitter.
 */
error_t parse_command(char *str, uint16_t buf_size, uint8_t access);

#endif /* APP_SHELL_IF_H_ */
/** @} */
