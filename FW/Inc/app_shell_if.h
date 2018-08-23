/*
 * MIT License
 *
 * Copyright 2018 Kevin Weiss
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

/*
 * @file serial_com.h
 * @author Kevin Weiss
 * @date 16.05.2018
 * @brief The file for managing basic serial communication.
 *
 * This initializes and runs the serial communication protocol for interfacing
 * to registers.  It used the STM HAL and a UART for the IO.  It also uses DMA.
 */

#ifndef APP_SHELL_IF_H_
#define APP_SHELL_IF_H_

#include <errno.h>
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define RX_END_CHAR		'\n'
#define TX_END_STR		"\n"

#define MODE_ECHO		0x00
#define MODE_ECHO_EXT	0x01
#define MODE_REG		0x02
#define MODE_TX			0x03

/* Public Functions ----------------------------------------------------------*/
/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
error_t parse_input(uint8_t mode, char *str, uint16_t buf_size, uint8_t access);

#endif /* APP_SHELL_IF_H_ */
