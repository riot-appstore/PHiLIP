/*
 * port_usb.h
 *
 *  Created on: Jul 11, 2018
 *      Author: kevinweiss
 */

#ifndef PORT_USB_H_
#define PORT_USB_H_

#include <errno.h>

typedef struct PORT_USB_TAG
{
	char *str;
	uint16_t size;
	uint16_t index;
	uint8_t access;
	uint8_t mode;
}PORT_USB_t;

error_t port_usb_init(PORT_USB_t *port_usb);
error_t port_usb_poll(PORT_USB_t *port_usb);

#endif /* PORT_USB_H_ */
