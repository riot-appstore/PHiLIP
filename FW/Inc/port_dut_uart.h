/*
 * app_uart.h
 *
 *  Created on: Jun 17, 2018
 *      Author: Yegor Yefremov
 */

#ifndef PORT_DUT_UART_H_
#define PORT_DUT_UART_H_

#include <errno.h>
#include "app_typedef.h"
#include "port_uart.h"

#define BPT_PARITY_NONE	0x00
#define BPT_PARITY_EVEN	0x01
#define BPT_PARITY_ODD	0x02

error_t uart_dut_execute(uart_t *uart);
error_t uart_dut_init(PORT_UART_t *huart);

#endif /* PORT_DUT_UART_H_ */
