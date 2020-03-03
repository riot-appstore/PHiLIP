/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @file			main.c
 * @author			Kevin Weiss
 * @author			Yegor Yefremov
 * @date			13.02.2019
 * @brief			Main program.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_errno.h"
#include "app_access.h"
#include "app_common.h"
#include "app_defaults.h"
#include "app_shell_if.h"
#include "app_reg.h"

#include "gpio.h"

#include "uart.h"

/* Private enums/structs -----------------------------------------------------*/
/** @brief  				The type of interface mode the uart is using */
enum UART_IF_TYPE {
	UART_IF_TYPE_ECHO, /**< uart echos */
	UART_IF_TYPE_ECHO_EXT, /**< uart echos and adds one */
	UART_IF_TYPE_REG, /**< uart reads application registers */
	UART_IF_TYPE_TX /**< uart constantly transmits */
};

/** @brief  				The amount of databits */
enum APP_UART_DATABITS {
	APP_UART_DATABITS_8, /**< 8 bits mode */
	APP_UART_DATABITS_7 /**< 7 bits mode */
};

/** @brief  				The uart paraity select */
enum APP_UART_PARITY {
	APP_UART_PARITY_NONE, /**< No parity */
	APP_UART_PARITY_EVEN, /**< Even parity */
	APP_UART_PARITY_ODD /**< Odd parity */
};

/** @brief						The parameters for uart control */
typedef struct {
	UART_HandleTypeDef huart; /**< Handle for the uart device */
	uart_t *reg; /**< uart live application registers */
	uart_t *saved_reg; /**< uart saved application registers */
	char *str; /**< String buffer */
	uint32_t buf_size; /**< String buffer size */
	uint8_t access; /**< Access level of uart */
} uart_dev_t;
/** @} */

/* Private defines -----------------------------------------------------------*/
/** @brief	Checks the uart register to see if any data is waiting */
#define IS_RX_WAITING(x)	(HAL_IS_BIT_SET(x, USART_CR3_DMAR))

/* Private function prototypes -----------------------------------------------*/
static error_t _commit_uart(uart_dev_t *dev);
static error_t _poll_uart(uart_dev_t *dev);
static error_t _tx_str(uart_dev_t *port_uart);
static error_t _rx_str(uart_dev_t *dev);
static error_t _xfer_complete(uart_dev_t *dev);
static inline int32_t _get_rx_amount(uart_dev_t *dev);

/* Global functions ----------------------------------------------------------*/
extern void _Error_Handler(char *, int);

/* Global variables ----------------------------------------------------------*/
/** @brief	dma handle for interface receive */
DMA_HandleTypeDef hdma_usart_if_rx;

/** @brief	dma handle for interface transmit */
DMA_HandleTypeDef hdma_usart_if_tx;

/** @brief	dma handle for dut receive */
DMA_HandleTypeDef hdma_usart_dut_rx;

/** @brief	dma handle for dut transmit */
DMA_HandleTypeDef hdma_usart_dut_tx;

/* Private variables ---------------------------------------------------------*/
static uart_dev_t dut_uart;
static uart_dev_t if_uart;
static char if_str_buf[UART_IF_BUF_SIZE];
static char dut_str_buf[UART_DUT_BUF_SIZE];


/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes dut uart registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates dut uart defaults registers and assigns uart register
 * 				pointers.
 */
void init_dut_uart(map_t *reg, map_t *saved_reg) {
	dut_uart.access = PERIPH_ACCESS;
	dut_uart.str = dut_str_buf;
	dut_uart.buf_size = sizeof(dut_str_buf)/sizeof(dut_str_buf[0]);
	memset(dut_uart.str, 0, dut_uart.buf_size);

	dut_uart.huart.Instance = DUT_UART;
	dut_uart.huart.Init.Mode = UART_MODE_TX_RX;
	dut_uart.huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	dut_uart.huart.Init.OverSampling = UART_OVERSAMPLING_16;

	dut_uart.reg = &reg->uart;
	dut_uart.saved_reg = &saved_reg->uart;

	dut_uart.reg->baud = DEFAULT_UART_BAUDRATE;

	_commit_uart(&dut_uart);

	sprintf(dut_uart.str, "{\"build_date\":\"%s %s\",\"result\":0}\n", __DATE__,
	__TIME__);
	_tx_str(&dut_uart);
}

/**
 * @brief		Initializes interface uart registers.
 *
 * @note		Populates dut uart defaults registers, uart register are NULL.
 */
void init_if_uart() {
	if_uart.access = IF_ACCESS;
	if_uart.str = if_str_buf;
	if_uart.buf_size = sizeof(if_str_buf)/sizeof(if_str_buf[0]);
	memset(if_uart.str, 0, if_uart.buf_size);

	if_uart.reg = NULL;
	if_uart.saved_reg = NULL;

	if_uart.huart.Instance = IF_UART;
	if_uart.huart.Init.BaudRate = 115200;
	if_uart.huart.Init.WordLength = UART_WORDLENGTH_8B;
	if_uart.huart.Init.StopBits = UART_STOPBITS_1;
	if_uart.huart.Init.Parity = UART_PARITY_NONE;
	if_uart.huart.Init.Mode = UART_MODE_TX_RX;
	if_uart.huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if_uart.huart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&if_uart.huart) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	sprintf(if_uart.str, "{\"build_date\":\"%s %s\",\"result\":0}\n", __DATE__,
		__TIME__);
	_tx_str(&if_uart);
}

/**
 * @brief		Commits the dut uart registers and executes operations.
 *
 * @pre			uart must first be initialized with init_dut_uart()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the uart.mode.init is clear.
 */
error_t commit_dut_uart() {
	return _commit_uart(&dut_uart);
}

static error_t _commit_uart(uart_dev_t *dev) {
	UART_HandleTypeDef *huart_inst = &(dev->huart);
	uart_t *uart = dev->reg;

	if (!uart->mode.init) {
		if (uart->mode.if_type > UART_IF_TYPE_TX) {
			return EINVAL;
		}
		if (uart->mode.data_bits == APP_UART_DATABITS_7 && !uart->mode.parity) {
			return EINVAL;
		}
		uart->mask_msb = 0xFF;
		huart_inst->Init.BaudRate = uart->baud;
		huart_inst->Init.StopBits =
				uart->mode.stop_bits ? UART_STOPBITS_2 : UART_STOPBITS_1;
		if (uart->mode.parity) {
			if (uart->mode.data_bits == APP_UART_DATABITS_7) {
				huart_inst->Init.WordLength = UART_WORDLENGTH_8B;
				uart->mask_msb = 0x7F;
			} else {
				huart_inst->Init.WordLength = UART_WORDLENGTH_9B;
			}

			if (uart->mode.parity == APP_UART_PARITY_EVEN)
				huart_inst->Init.Parity = UART_PARITY_EVEN;
			else
				huart_inst->Init.Parity = UART_PARITY_ODD;
		} else {
			huart_inst->Init.Parity = UART_PARITY_NONE;
			huart_inst->Init.WordLength = UART_WORDLENGTH_8B;
		}

		if (uart->mode.rts) {
			HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_RESET);
		}
		if (!dev->reg->mode.disable) {
			if (HAL_UART_Init(&dut_uart.huart) != HAL_OK) {
				_Error_Handler(__FILE__, __LINE__);
			}
		}
		else {
			HAL_UART_DeInit(huart_inst);
			if (init_basic_gpio(dev->reg->dut_tx, DUT_TX) != EOK) {
				return EINVAL;
			}
			if (init_basic_gpio(dev->reg->dut_rx, DUT_RX) != EOK) {
				return EINVAL;
			}
			if (init_basic_gpio(dev->reg->dut_cts, DUT_CTS) != EOK) {
				return EINVAL;
			}
			if (init_basic_gpio(dev->reg->dut_rts, DUT_RTS) != EOK) {
				return EINVAL;
			}
		}


		uart->rx_count = 0;
		uart->tx_count = 0;
		memset(&uart->status, 0, sizeof(uart->status));

		uart->mode.init = 1;
		copy_until_same(dev->saved_reg, dev->reg, sizeof(*(dev->saved_reg)));

		return EOK;
	}
	return ENOACTION;
}

void update_dut_uart_inputs() {
	dut_uart.reg->dut_tx.level = HAL_GPIO_ReadPin(DUT_TX);
	dut_uart.reg->dut_rx.level = HAL_GPIO_ReadPin(DUT_RX);
	dut_uart.reg->dut_cts.level = HAL_GPIO_ReadPin(DUT_CTS);
	dut_uart.reg->dut_rts.level = HAL_GPIO_ReadPin(DUT_RTS);
}

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
error_t poll_dut_uart() {
	return _poll_uart(&dut_uart);
}

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
error_t poll_if_uart() {
	return _poll_uart(&if_uart);
}

static error_t _poll_uart(uart_dev_t *dev) {
	error_t err = ENOACTION;
	UART_HandleTypeDef *huart = &(dev->huart);

	if (IS_RX_WAITING(huart->Instance->CR3)
			&& (dev->saved_reg == NULL
					|| dev->saved_reg->mode.if_type != UART_IF_TYPE_TX)) {
		err = _rx_str(dev);
	} else if (huart->TxXferCount == 0) {
		if (dev->str[dev->buf_size - 1] != 0) {

			HAL_UART_AbortTransmit(huart);
			HAL_UART_AbortReceive(huart);
			/* Parses the bufsize too big message */
			parse_command(dev->str, dev->buf_size, dev->access);
			dev->str[dev->buf_size - 1] = 0;
			HAL_UART_Transmit_DMA(huart, (uint8_t*) dev->str, strlen(dev->str));
		} else {
			err = _xfer_complete(dev);
		}
	}
	return err;
}

static error_t _xfer_complete(uart_dev_t *dev) {
	char *str = dev->str;
	UART_HandleTypeDef *huart = &dev->huart;

	HAL_StatusTypeDef status = HAL_ERROR;
	error_t err = EUNKNOWN;

	memset(str, 0, dev->buf_size);
	HAL_UART_AbortTransmit(huart);
	HAL_UART_AbortReceive(huart);
	if (dev->saved_reg != NULL) {
		if (dev->saved_reg->mode.if_type == UART_IF_TYPE_TX) {
			memset(str, 'a', dev->buf_size - 3);
			str[dev->buf_size - 2] = '\n';
			str[dev->buf_size - 1] = '\0';
			return _tx_str(dev);

		}
	}
	status = HAL_UART_Receive_DMA(huart, (uint8_t*) str, dev->buf_size);
	if (status == HAL_BUSY) {
		err = EBUSY;
	}
	if (status == HAL_ERROR) {
		err = ENXIO;
	} else if (status == HAL_OK) {
		err = EOK;
	}

	return err;
}

static error_t _update_tx_count(uart_dev_t *dev, uint16_t tx_amount) {
	error_t err = EOK;
	if (dev->reg != NULL) {
		//TODO: Look out for enable/disable interrupts... may cause jitter.
		DIS_INT;
		dev->reg->tx_count += tx_amount;
		EN_INT;
	}

	return err;
}

static error_t _update_rx_count(uart_dev_t *dev, uint16_t rx_amount) {
	error_t err = EOK;

	if (dev->reg != NULL) {
		//TODO: Look out for enable/disable interrupts... may cause jitter.
		DIS_INT;
		dev->reg->rx_count += rx_amount;
		EN_INT;
	}
	return err;
}

static error_t _rx_str(uart_dev_t *dev) {
	char *str = dev->str;
	UART_HandleTypeDef *huart = &(dev->huart);

	uint16_t rx_amount;
	error_t err = ENOACTION;

	rx_amount = _get_rx_amount(dev);
	if (rx_amount >= 1) {
		if (dev->saved_reg != NULL) {
			for (int i = 0; i < strlen(str); i++) {
				str[i] &= dev->saved_reg->mask_msb;
			}
		}
		if (str[rx_amount - 1] == RX_END_CHAR
				&& _get_rx_amount(dev) != dev->buf_size) {

			_update_rx_count(dev, strlen(str));
			HAL_UART_AbortTransmit(huart);
			HAL_UART_AbortReceive(huart);
			if (rx_amount >= 2 && str[rx_amount - 2] == '\r'){
				str[rx_amount - 2] = '\n';
				str[rx_amount - 1] = 0;
			}
			if (dev->saved_reg == NULL
					|| dev->saved_reg->mode.if_type == UART_IF_TYPE_REG) {
				err = parse_command(str, dev->buf_size, dev->access);
			} else if (dev->saved_reg->mode.if_type == UART_IF_TYPE_ECHO) {
				err = EOK;
			} else if (dev->saved_reg->mode.if_type == UART_IF_TYPE_ECHO_EXT) {
				int i;
				for (i = 0; i < strlen(str) - 1; i++) {
					str[i]++;
				}
				err = EOK;
			} else {
				err = EPROTONOSUPPORT;
			}
			_update_tx_count(dev, strlen(str));
			HAL_UART_Transmit_DMA(huart, (uint8_t*) str, strlen(str));

		}
	}
	return err;
}

static error_t _tx_str(uart_dev_t *dev) {
	char *str = dev->str;
	UART_HandleTypeDef *huart = &(dev->huart);
	error_t err = EUNKNOWN;
	HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(huart, (uint8_t*) str,
			strlen(str));

	if (status == HAL_BUSY) {
		HAL_UART_AbortTransmit(huart);
		HAL_UART_AbortReceive(huart);
		status = HAL_UART_Transmit_DMA(huart, (uint8_t*) str, strlen(str));
		if (status == HAL_BUSY) {
			err = EBUSY;
		}
	}
	if (status == HAL_ERROR) {
		err = ENXIO;
	} else if (status == HAL_OK) {
		err = EOK;
	}

	return err;
}

static inline int32_t _get_rx_amount(uart_dev_t *dev) {
	return (dev->buf_size - dev->huart.hdmarx->Instance->CNDTR);
}

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief This function handles dut_dma_tx event interrupt.
 */
void DUT_UART_DMA_TX_INT(void) {
	HAL_DMA_IRQHandler(&hdma_usart_dut_tx);
}

/**
 * @brief This function handles dut_dma_rx event interrupt.
 */
void DUT_UART_DMA_RX_INT(void) {
	HAL_DMA_IRQHandler(&hdma_usart_dut_rx);
}

/**
 * @brief This function handles if_dma_rx event interrupt.
 */
void IF_UART_DMA_RX_INT(void) {
	HAL_DMA_IRQHandler(&hdma_usart_if_rx);
}

/**
 * @brief This function handles if_dma_tx event interrupt.
 */
void IF_UART_DMA_TX_INT(void) {
	HAL_DMA_IRQHandler(&hdma_usart_if_tx);
}

/**
 * @brief This function handles dut_uart event interrupt.
 */
void DUT_UART_INT(void) {
	HAL_UART_IRQHandler(&dut_uart.huart);

	if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_PE) {
		dut_uart.reg->status.pe = 1;
	} else if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_FE) {
		dut_uart.reg->status.fe = 1;
	} else if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_NE) {
		dut_uart.reg->status.nf = 1;
	} else if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_ORE) {
		dut_uart.reg->status.ore = 1;
	}
}

/**
 * @brief This function handles dut_cts event interrupt.
 */
void dut_cts_int() {
	dut_uart.reg->status.cts = 1;
}
