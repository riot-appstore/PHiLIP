/*
 * port_usb.c
 *
 *  Created on: Jul 11, 2018
 *      Author: kevinweiss
 */
#include <string.h>

#include "stm32f1xx_hal.h"
#include "usbd_def.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#include "port_usb.h"
#include "app_shell_if.h"

#include "app_access.h"
#include "app_errno.h"
#include "app_typedef.h"
#include "app_common.h"
#include "app.h"
#include "app_debug.h"


static error_t _rx_str(PORT_USB_t *port_usb);

static void _clear_usb(PORT_USB_t *port_usb);

/**
 * @brief Polls and parses the rx and tx buffers.
 *
 * @retval errno defined error code.
 *
 * May need work to make interrupts better
 * problem resetting, must be replugged in order to init
 */
error_t port_usb_poll(PORT_USB_t *port_usb) {
	error_t err = ENOACTION;
#if 1
	USBD_CDC_HandleTypeDef *hcdc;
	hcdc = get_cdc_handle();
	if (hcdc != NULL) {
		if (hcdc->RxLength > 0) {
			DIS_INT;
			if (hcdc->RxLength + port_usb->index < port_usb->size) {
				memcpy(&port_usb->str[port_usb->index], hcdc->RxBuffer, hcdc->RxLength);
				port_usb->index += hcdc->RxLength;
				hcdc->RxLength = 0;
			}
			else {
				_clear_usb(port_usb);
			}
			EN_INT;
			_rx_str(port_usb);
		}
		else if (hcdc->TxLength > 0) {
			if (hcdc->TxState == 0) {
				_clear_usb(port_usb);
			}
		}
	}
#endif
	return err;
}

/**
 * @brief Polls and parses the rx and tx buffers.
 *
 * @retval errno defined error code.
 */
static void _clear_usb(PORT_USB_t *port_usb) {

	EN_INT;
	USBD_CDC_HandleTypeDef *hcdc;
	hcdc = get_cdc_handle();
	memset(port_usb->str, 0, port_usb->size);
	port_usb->index = 0;
	DIS_INT;
	hcdc->RxLength = 0;
	hcdc->TxLength = 0;
	EN_INT;
}

/**
 * @brief Polls and parses the rx and tx buffers.
 *
 * @retval errno defined error code.
 */
static error_t _rx_str(PORT_USB_t *port_usb) {
	char *str = port_usb->str;

	error_t err = ENOACTION;
	if (port_usb->index > 0) {
		if (str[port_usb->index - 1] == RX_END_CHAR) {
			err = parse_input(port_usb->mode, str, port_usb->size, port_usb->access);
			CDC_Transmit_FS((uint8_t*) str, strlen(str));
		}
	}
	return err;
}
