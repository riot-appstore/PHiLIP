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
 ******************************************************************************
 * @file           : port_dut_uart.c
 * @author         : Yegor Yefremov
 * @date           : 16.06.2018
 * @brief          : Application specific UART functionality.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "app_errno.h"
#include "app_common.h"
#include "app_errno.h"
#include "app.h"
#include "app_debug.h"
#include "port_dut_uart.h"
#include "app_shell_if.h"


/* Private variables ---------------------------------------------------------*/
static PORT_UART_t *uart_dut_inst;

/**
 * @brief Initializes and attaches all the pointers for communication.
 *
 * @retval errno defined error code.
 */
error_t uart_dut_init(PORT_UART_t *uart_inst) {
	uart_dut_inst = uart_inst;
	return EOK;
}

/**
 * @brief
 *
 * @retval errno defined error code.
 */
error_t uart_dut_execute(uart_t *uart) {
	uart_dut_inst->mode = uart->mode;

	uart_dut_inst->huart->Init.BaudRate = uart->baud;
	uart_dut_inst->huart->Init.StopBits = uart->ctrl.stop_bits ? UART_STOPBITS_2 : UART_STOPBITS_1;
	switch (uart->ctrl.parity) {
	case BPT_PARITY_NONE:
		uart_dut_inst->huart->Init.Parity = UART_PARITY_NONE;
		uart_dut_inst->huart->Init.WordLength = UART_WORDLENGTH_8B;
		break;
	case BPT_PARITY_EVEN:
		uart_dut_inst->huart->Init.Parity = UART_PARITY_EVEN;
		uart_dut_inst->huart->Init.WordLength = UART_WORDLENGTH_9B;
		break;
	case BPT_PARITY_ODD:
		uart_dut_inst->huart->Init.Parity = UART_PARITY_ODD;
		uart_dut_inst->huart->Init.WordLength = UART_WORDLENGTH_9B;
		break;
	}

	DIS_INT;
	HAL_UART_Init(uart_dut_inst->huart);
	EN_INT;

	if (uart->ctrl.rts)
		HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_RESET);

	return EOK;
}
