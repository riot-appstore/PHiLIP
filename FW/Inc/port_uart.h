/*
 * port_uart_if.h
 *
 *  Created on: Jul 2, 2018
 *      Author: kevinweiss
 */

#ifndef PORT_UART_H_
#define PORT_UART_H_

#include <errno.h>

typedef struct PORT_UART_TAG
{
	UART_HandleTypeDef *huart;
	char *str;
	uint16_t size;
	uint8_t access;
	uint8_t mode;
}PORT_UART_t;

error_t port_uart_init(PORT_UART_t *port_uart);
error_t port_uart_poll(PORT_UART_t *port_uart);

#endif /* PORT_UART_H_ */
