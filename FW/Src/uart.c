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
 * @file			uart.c
 * @author			Kevin Weiss
 * @author			Yegor Yefremov
 * @date			13.02.2019
 * @brief			Controls the uart peripherals.
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_access.h"
#include "app_common.h"
#include "app_defaults.h"
#include "app_shell_if.h"
#include "app_reg.h"

#include "gpio.h"

#include "uart.h"

/* Private enums/structs ******************************************************/
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
	/** @brief Function pointer to the tx (either DMA or INT) */
	HAL_StatusTypeDef (*tx_data_fxn)(UART_HandleTypeDef*, uint8_t*, uint16_t);
	char *str; /**< String buffer */
	uint32_t buf_size; /**< String buffer size */
	uint8_t mask_msb; /**< Used for masking bits */
	uart_mode_t mode; /**< Mode of uart */
	uint8_t access; /**< Access level of uart */
} uart_dev_t;
/** @} */

/* Private macros *************************************************************/
/** @brief	Checks the uart register to see if any data is waiting */
#define IS_RX_WAITING(x)	(HAL_IS_BIT_SET(x, USART_CR3_DMAR))

/* Private function prototypes ************************************************/
static void _init_gpio();

static error_t _poll_uart(uart_dev_t *dev);
static error_t _tx_str(uart_dev_t *port_uart);
static error_t _rx_str(uart_dev_t *dev);
static error_t _xfer_complete(uart_dev_t *dev);
static inline int32_t _get_rx_amount(uart_dev_t *dev);
static inline void _update_tx_count(uart_dev_t* dev, uint16_t tx_amount);
static inline void _update_rx_count(uart_dev_t* dev, uint16_t rx_amount);

/* Global functions ***********************************************************/
extern void _Error_Handler(char *, int);

/* Private variables **********************************************************/
static uart_dev_t dut_uart = {0};
static uart_dev_t if_uart = {0};

static char if_str_buf[UART_IF_BUF_SIZE] = {0};

static char dut_str_buf[UART_DUT_BUF_SIZE] = {0};

static uart_t* dut_uart_reg;

/** @brief	dma handle for interface receive */
static DMA_HandleTypeDef hdma_usart_if_rx;

/** @brief	dma handle for interface transmit */
static DMA_HandleTypeDef hdma_usart_if_tx;

/** @brief	dma handle for dut receive */
static DMA_HandleTypeDef hdma_usart_dut_rx;

/******************************************************************************/
/*           Initialization DUT_UART                                          */
/******************************************************************************/
void init_dut_uart(map_t *reg) {
	uart_dev_t* uart_dev = &dut_uart;

	_init_gpio();

	uart_dev->tx_data_fxn = HAL_UART_Transmit_IT;
	uart_dev->access = PERIPH_ACCESS;
	uart_dev->str = dut_str_buf;
	uart_dev->buf_size = sizeof(dut_str_buf)/sizeof(dut_str_buf[0]);

	uart_dev->huart.Instance = DUT_UART_INST;
	uart_dev->huart.Init.Mode = UART_MODE_TX_RX;
	uart_dev->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_dev->huart.Init.OverSampling = UART_OVERSAMPLING_16;

	dut_uart_reg = &reg->uart;
	dut_uart_reg->baud = DEFAULT_UART_BAUDRATE;

	commit_dut_uart();

	sprintf(uart_dev->str, "{\"build_date\":\"%s %s\",\"result\":0}\n",
			__DATE__, __TIME__);
	_tx_str(uart_dev);
}

/******************************************************************************/
void init_dut_uart_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	UART_HandleTypeDef* huart = &dut_uart.huart;
	DUT_UART_CLK_EN();

	GPIO_InitStruct.Pin = DUT_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_RX_Pin;
	//GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	hdma_usart_dut_rx.Instance = DUT_UART_RX_DMA_INST;
	hdma_usart_dut_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_usart_dut_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_usart_dut_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_usart_dut_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_usart_dut_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_usart_dut_rx.Init.Mode = DMA_NORMAL;
	hdma_usart_dut_rx.Init.Priority = DMA_PRIORITY_LOW;

	if (HAL_DMA_Init(&hdma_usart_dut_rx) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	__HAL_LINKDMA(huart, hdmarx, hdma_usart_dut_rx);

	HAL_NVIC_SetPriority(DUT_UART_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(DUT_UART_IRQ);

    HAL_NVIC_SetPriority(DUT_UART_DMA_RX_IRQ, DEFAULT_INT_PRIO, 0);
    HAL_NVIC_EnableIRQ(DUT_UART_DMA_RX_IRQ);
}

void deinit_dut_uart_msp() {
	UART_HandleTypeDef* huart = &dut_uart.huart;
	DUT_UART_CLK_DIS();
	HAL_GPIO_DeInit(DUT_RX);
	HAL_GPIO_DeInit(DUT_TX);
	HAL_DMA_DeInit(huart->hdmarx);
	HAL_NVIC_DisableIRQ(DUT_UART_IRQ);
}

/******************************************************************************/
static void _init_gpio() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Pin = DUT_RTS_Pin;
	HAL_GPIO_Init(DUT_RTS_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_CTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(DUT_CTS_GPIO_Port, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(GPIO_CTS_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(GPIO_CTS_IRQ);
}
/******************************************************************************/
error_t commit_dut_uart() {
	UART_HandleTypeDef* huart = &dut_uart.huart;
	uart_t *reg = dut_uart_reg;

	if (reg->mode.init) {
		return 0;
	}
	if (reg->mode.if_type > UART_IF_TYPE_TX) {
		return EINVAL;
	}
	if (reg->mode.data_bits == APP_UART_DATABITS_7 && !reg->mode.parity) {
		return EINVAL;
	}
	dut_uart.mask_msb = 0xFF;
	huart->Init.BaudRate = reg->baud;
	huart->Init.StopBits = reg->mode.stop_bits ? UART_STOPBITS_2 : UART_STOPBITS_1;
	if (reg->mode.parity) {
		if (reg->mode.data_bits == APP_UART_DATABITS_7) {
			huart->Init.WordLength = UART_WORDLENGTH_8B;
			dut_uart.mask_msb = 0x7F;
		} else {
			huart->Init.WordLength = UART_WORDLENGTH_9B;
		}

		if (reg->mode.parity == APP_UART_PARITY_EVEN)
			huart->Init.Parity = UART_PARITY_EVEN;
		else
			huart->Init.Parity = UART_PARITY_ODD;
	} else {
		huart->Init.Parity = UART_PARITY_NONE;
		huart->Init.WordLength = UART_WORDLENGTH_8B;
	}

	if (reg->mode.rts) {
		HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_RESET);
	}
	if (!reg->mode.disable) {
		if (HAL_UART_Init(huart) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}
	}
	else {
		HAL_UART_DeInit(huart);
		if (init_basic_gpio(reg->dut_tx, DUT_TX) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_rx, DUT_RX) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_cts, DUT_CTS) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_rts, DUT_RTS) != 0) {
			return EINVAL;
		}
	}

	reg->rx_count = 0;
	reg->tx_count = 0;

	reg->status.cts = 0;
	reg->status.fe = 0;
	reg->status.nf = 0;
	reg->status.ore = 0;
	reg->status.pe = 0;

	reg->mode.init = 1;

	return 0;
}

/******************************************************************************/
/*           Initialization IF_UART                                           */
/******************************************************************************/
void init_if_uart() {
	uart_dev_t* uart_dev = &if_uart;
	uart_dev->tx_data_fxn = HAL_UART_Transmit_DMA;

	uart_dev->access = IF_ACCESS;
	uart_dev->str = if_str_buf;
	uart_dev->buf_size = sizeof(if_str_buf)/sizeof(if_str_buf[0]);
	memset(if_uart.str, 0, if_uart.buf_size);

	uart_dev->huart.Instance = IF_UART_INST;
	uart_dev->huart.Init.BaudRate = DEFAULT_UART_BAUDRATE;
	uart_dev->huart.Init.WordLength = UART_WORDLENGTH_8B;
	uart_dev->huart.Init.StopBits = UART_STOPBITS_1;
	uart_dev->huart.Init.Parity = UART_PARITY_NONE;
	uart_dev->huart.Init.Mode = UART_MODE_TX_RX;
	uart_dev->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_dev->huart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&uart_dev->huart) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	uart_dev->mask_msb = 0xFF;
	uart_dev->mode.if_type = UART_IF_TYPE_REG;

	sprintf(uart_dev->str, "{\"build_date\":\"%s %s\",\"result\":0}\n",
			__DATE__, __TIME__);
	_tx_str(uart_dev);
}

void init_if_uart_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	UART_HandleTypeDef* huart = &if_uart.huart;
	DMA_HandleTypeDef* huart_rx_dma = &hdma_usart_if_rx;
	DMA_HandleTypeDef* huart_tx_dma = &hdma_usart_if_tx;

	IF_UART_CLK_EN();

	GPIO_InitStruct.Pin = IF_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(IF_TX_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = IF_RX_Pin;
	//GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(IF_RX_GPIO_Port, &GPIO_InitStruct);

	huart_rx_dma->Instance = IF_UART_DMA_RX_INST;
	huart_rx_dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
	huart_rx_dma->Init.PeriphInc = DMA_PINC_DISABLE;
	huart_rx_dma->Init.MemInc = DMA_MINC_ENABLE;
	huart_rx_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	huart_rx_dma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	huart_rx_dma->Init.Mode = DMA_NORMAL;
	huart_rx_dma->Init.Priority = DMA_PRIORITY_LOW;
	if (HAL_DMA_Init(huart_rx_dma) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	__HAL_LINKDMA(huart, hdmarx, (*huart_rx_dma));

	huart_tx_dma->Instance = IF_UART_DMA_TX_INST;
	huart_tx_dma->Init.Direction = DMA_MEMORY_TO_PERIPH;
	huart_tx_dma->Init.PeriphInc = DMA_PINC_DISABLE;
	huart_tx_dma->Init.MemInc = DMA_MINC_ENABLE;
	huart_tx_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	huart_tx_dma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	huart_tx_dma->Init.Mode = DMA_NORMAL;
	huart_tx_dma->Init.Priority = DMA_PRIORITY_LOW;
	if (HAL_DMA_Init(huart_tx_dma) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	__HAL_LINKDMA(huart, hdmatx, (*huart_tx_dma));

	HAL_NVIC_SetPriority(IF_UART_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(IF_UART_IRQ);

	HAL_NVIC_SetPriority(IF_UART_DMA_RX_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(IF_UART_DMA_RX_IRQ);
	HAL_NVIC_SetPriority(IF_UART_DMA_TX_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(IF_UART_DMA_TX_IRQ);
}

void deinit_if_uart_msp() {
	UART_HandleTypeDef* huart = &if_uart.huart;
	IF_UART_CLK_DIS();
	HAL_GPIO_DeInit(IF_RX_GPIO_Port, IF_RX_Pin);
	HAL_GPIO_DeInit(IF_TX_GPIO_Port, IF_TX_Pin);
	HAL_DMA_DeInit(huart->hdmarx);
	HAL_NVIC_DisableIRQ(IF_UART_IRQ);
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
error_t poll_dut_uart() {
	return _poll_uart(&dut_uart);
}

error_t poll_if_uart() {
	return _poll_uart(&if_uart);
}

static error_t _poll_uart(uart_dev_t *dev) {
	error_t err = 0;
	UART_HandleTypeDef *huart = &(dev->huart);

	if (IS_RX_WAITING(huart->Instance->CR3) && (dev->mode.if_type != UART_IF_TYPE_TX)) {
		err = _rx_str(dev);
	}
	else if (huart->TxXferCount == 0) {
		if (dev->str[dev->buf_size - 1] != 0) {
			HAL_UART_Abort(huart);
			parse_command(dev->str, dev->buf_size, dev->access);
			dev->str[dev->buf_size - 1] = 0;
			dev->tx_data_fxn(huart, (uint8_t*) dev->str, strlen(dev->str));
		}
		else {
			err = _xfer_complete(dev);
		}
	}
	return err;
}

/******************************************************************************/
static error_t _xfer_complete(uart_dev_t *dev) {
	char *str = dev->str;
	UART_HandleTypeDef *huart = &dev->huart;

	HAL_StatusTypeDef status = HAL_ERROR;
	error_t err = ENXIO;

	memset(str, 0, dev->buf_size);
	HAL_UART_Abort(huart);
	if (dev->mode.if_type == UART_IF_TYPE_TX) {
		memset(str, 'a', dev->buf_size - 3);
		str[dev->buf_size - 2] = '\n';
		str[dev->buf_size - 1] = '\0';
		return _tx_str(dev);
	}

	status = HAL_UART_Receive_DMA(huart, (uint8_t*) str, dev->buf_size);
	if (status == HAL_BUSY) {
		err = EBUSY;
	}
	if (status == HAL_ERROR) {
		err = ENXIO;
	}
	else if (status == HAL_OK) {
		err = 0;
	}
	return err;
}

static error_t _rx_str(uart_dev_t *dev) {
	char *str = dev->str;
	UART_HandleTypeDef *huart = &(dev->huart);

	uint16_t rx_amount;
	error_t err = 0;

	rx_amount = _get_rx_amount(dev);
	if (rx_amount >= 1) {
		for (int i = 0; i < strlen(str); i++) {
			str[i] &= dev->mask_msb;
		}
		if (str[rx_amount - 1] == RX_END_CHAR && _get_rx_amount(dev) != dev->buf_size) {
			_update_rx_count(dev, strlen(str));
			HAL_UART_Abort(huart);

			if (rx_amount >= 2 && str[rx_amount - 2] == '\r'){
				str[rx_amount - 2] = '\n';
				str[rx_amount - 1] = 0;
			}
			if (dev->mode.if_type == UART_IF_TYPE_REG) {
				err = parse_command(str, dev->buf_size, dev->access);
			} else if (dev->mode.if_type == UART_IF_TYPE_ECHO) {
				err = 0;
			} else if (dev->mode.if_type == UART_IF_TYPE_ECHO_EXT) {
				int i;
				for (i = 0; i < strlen(str) - 1; i++) {
					str[i]++;
				}
				err = 0;
			} else {
				err = EPROTONOSUPPORT;
			}
			_update_tx_count(dev, strlen(str));
			dev->tx_data_fxn(huart, (uint8_t*) str, strlen(str));
		}
	}
	return err;
}

static error_t _tx_str(uart_dev_t *dev) {
	char *str = dev->str;
	UART_HandleTypeDef *huart = &(dev->huart);
	error_t err = ENXIO;
	HAL_StatusTypeDef status = dev->tx_data_fxn(huart, (uint8_t*) str,
			strlen(str));

	if (status == HAL_BUSY) {
		HAL_UART_Abort(huart);
		status = dev->tx_data_fxn(huart, (uint8_t*) str, strlen(str));
		if (status == HAL_BUSY) {
			err = EBUSY;
		}
	}
	if (status == HAL_ERROR) {
		err = ENXIO;
	} else if (status == HAL_OK) {
		err = 0;
	}
	return err;
}
/******************************************************************************/

static inline void _update_tx_count(uart_dev_t* dev, uint16_t tx_amount) {
	if (dev->huart.Instance == DUT_UART_INST) {
		dut_uart_reg->tx_count += tx_amount;
	}
}

static inline void _update_rx_count(uart_dev_t* dev, uint16_t rx_amount) {
	if (dev->huart.Instance == DUT_UART_INST) {
		dut_uart_reg->rx_count += rx_amount;
	}
}

static inline int32_t _get_rx_amount(uart_dev_t *dev) {
	return (dev->buf_size - dev->huart.hdmarx->Instance->CNDTR);
}

/******************************************************************************/
void update_dut_uart_inputs() {
	dut_uart_reg->dut_tx.level = HAL_GPIO_ReadPin(DUT_TX);
	dut_uart_reg->dut_rx.level = HAL_GPIO_ReadPin(DUT_RX);
	dut_uart_reg->dut_cts.level = HAL_GPIO_ReadPin(DUT_CTS);
	dut_uart_reg->dut_rts.level = HAL_GPIO_ReadPin(DUT_RTS);
}

/******************************************************************************/
/*           Interrupt Handling                                               */
/******************************************************************************/
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
		dut_uart_reg->status.pe = 1;
	} else if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_FE) {
		dut_uart_reg->status.fe = 1;
	} else if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_NE) {
		dut_uart_reg->status.nf = 1;
	} else if (dut_uart.huart.ErrorCode & HAL_UART_ERROR_ORE) {
		dut_uart_reg->status.ore = 1;
	}
}

/**
 * @brief This function handles if_uart event interrupt.
 */
void IF_UART_INT(void) {
	HAL_UART_IRQHandler(&if_uart.huart);
}

/**
 * @brief This function handles dut_cts event interrupt.
 */
void GPIO_CTS_INT() {
	dut_uart_reg->status.cts = 1;
}
