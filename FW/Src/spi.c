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
 * @file			spi.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Controls the spi peripheral.
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_access.h"
#include "app_common.h"
#include "app_reg.h"
#include "gpio.h"

#include "spi.h"

/* Private enums/structs ******************************************************/
/** @brief  							The state settings of the SPI */
enum SPI_STATE {
	SPI_INITIALIZED, /**< spi has been initialized */
	SPI_FRAME_STARTED, /**< spi NSS pin just lowered */
	SPI_WRITING, /**< spi writing to reg */
	SPI_READING, /**< spi reading reg */
	SPI_TRANSFERING, /**< spi transferring data */
	SPI_FRAME_FINISHED /**< spi cs pin up and finished */
};

/** @brief	Interface type for the spi */
enum SPI_IF_TYPE {
	SPI_IF_TYPE_REG, /**< Access registers with spi */
	SPI_IF_TYPE_HS, /**< Preloads reg address to 0 for high speed tests */
	SPI_IF_TYPE_ECHO, /**< Echos SPI bytes */
	SPI_IF_TYPE_CONST, /**< Always output user reg 0 */
	SPI_IF_TYPE_SM /**< Use timer input capture to measure spi clock speed */
};

/** @brief	The parameters for spi control */
typedef struct {
	SPI_HandleTypeDef hspi; /**< Handle for the spi device */
	TIM_HandleTypeDef htmr; /**< Handle for the tmr device */
	DMA_HandleTypeDef htmr_dma; /**< Handle for the tmr dma */
	TIM_IC_InitTypeDef ctmr_ic; /**< Initialization for timer */
	spi_t *reg; /**< spi live application registers */
	uint16_t buf[64]; /**< Buffer for DMA */
	int8_t poll_index; /**< Buffer index when polling */
	void (*if_mode_int)(void); /**< Interrupt function pointer */
	uint8_t initial_byte; /**< The byte that is output first */
	uint8_t sm_active;
} spi_dev;
/** @} */

/* Private defines ************************************************************/
/** @brief	Checks the write direction */
#define SPI_ADDR_MASK	(0x80)
#define BITS_PER_BYTE	8

/* Private function prototypes ************************************************/
static void _init_gpio();

static void _spi_echo_int();
static void _spi_hs_int();
static void _spi_reg_int();
static void _spi_const_int();
static void _init_periph_spi_ic();
static void _deinit_periph_spi_ic(TIM_HandleTypeDef *htmr);

/* Private variables **********************************************************/
static spi_dev dut_spi;

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_dut_spi(map_t *reg) {
	SPI_HandleTypeDef *hspi = &dut_spi.hspi;

	_init_gpio();

	hspi->Instance = DUT_SPI_INST;
	hspi->Init.Mode = SPI_MODE_SLAVE;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	hspi->Init.NSS = SPI_NSS_HARD_INPUT;
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial = 0;

	dut_spi.reg = &(reg->spi);
	commit_dut_spi();
}

/******************************************************************************/
void init_dut_spi_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	DUT_SPI_CLK_EN();
	DUT_SPI_GPIO_CLK_EN();

	GPIO_InitStruct.Pin = DUT_NSS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DUT_NSS_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_SCK_Pin;
	HAL_GPIO_Init(DUT_SCK_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_MOSI_Pin;
	HAL_GPIO_Init(DUT_MOSI_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_MISO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;

	HAL_GPIO_Init(DUT_MISO_GPIO_Port, &GPIO_InitStruct);

	DUT_SPI_GPIO_AF_REMAP();

	HAL_NVIC_SetPriority(DUT_SPI_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(DUT_SPI_IRQ);
}

void deinit_dut_spi_msp() {
	DUT_SPI_CLK_DIS();

	HAL_GPIO_DeInit(DUT_NSS);
	HAL_GPIO_DeInit(DUT_SCK);
	HAL_GPIO_DeInit(DUT_MISO);
	HAL_GPIO_DeInit(DUT_MOSI);

	HAL_NVIC_DisableIRQ(DUT_SPI_IRQ);
}

/******************************************************************************/
/**
 * @brief Init timer and DMA peripherals used for the time measurements
 */
static void _init_periph_spi_ic() {
	TIM_HandleTypeDef *htmr = &(dut_spi.htmr);
	TIM_IC_InitTypeDef* ctmr_ic = &(dut_spi.ctmr_ic);
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	HAL_NVIC_SetPriority(DUT_SPI_IC_DMA_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(DUT_SPI_IC_DMA_IRQ);

	htmr->Instance = DUT_SPI_IC_INST;
	htmr->Init.Prescaler = 0;
	htmr->Init.CounterMode = TIM_COUNTERMODE_UP;
	htmr->Init.Period = 0xFFFF;
	htmr->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htmr->Init.RepetitionCounter = 0;
	htmr->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(htmr) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(htmr, &sClockSourceConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	if (HAL_TIM_IC_Init(htmr) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htmr, &sMasterConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	ctmr_ic->ICSelection = TIM_ICSELECTION_DIRECTTI;
	ctmr_ic->ICPrescaler = TIM_ICPSC_DIV1;
	ctmr_ic->ICFilter = 0;
}

static void _deinit_periph_spi_ic(TIM_HandleTypeDef *htmr){
	if(dut_spi.sm_active){
		HAL_TIM_IC_Stop_DMA(htmr, DUT_SPI_IC_CHANNEL);
		if (HAL_TIM_Base_DeInit(htmr) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}
		_init_gpio();
		dut_spi.sm_active = 0;
	}
}

void init_dut_spi_ic_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_HandleTypeDef *htmr = &(dut_spi.htmr);
	DMA_HandleTypeDef *htmr_dma = &(dut_spi.htmr_dma);

	/* Peripheral clock enable */
	DUT_SPI_IC_CLK_EN();
	DUT_SPI_IC_GPIO_CLK_EN();

	GPIO_InitStruct.Pin = DUT_SCK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DUT_SCK_GPIO_Port, &GPIO_InitStruct);

	DUT_SPI_IC_AFIO_REMAP();

	htmr_dma->Instance = DUT_SPI_IC_DMA_INST;
	htmr_dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
	htmr_dma->Init.PeriphInc = DMA_PINC_DISABLE;
	htmr_dma->Init.MemInc = DMA_MINC_ENABLE;
	htmr_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	htmr_dma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	htmr_dma->Init.Mode = DMA_NORMAL;
	htmr_dma->Init.Priority = DMA_PRIORITY_LOW;
	if (HAL_DMA_Init(htmr_dma) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	__HAL_LINKDMA(htmr,hdma[DUT_SPI_IC_DMA_ID],*htmr_dma);
	HAL_NVIC_SetPriority(DUT_SPI_IC_INT, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(DUT_SPI_IC_INT);
}

void deinit_dut_spi_ic_msp() {
	TIM_HandleTypeDef *htmr = &(dut_spi.htmr);

	DUT_SPI_IC_CLK_DIS();
	HAL_GPIO_DeInit(DUT_SCK);
	HAL_DMA_DeInit(htmr->hdma[DUT_SPI_IC_DMA_ID]);

	HAL_NVIC_DisableIRQ(DUT_SPI_IC_INT);
}

/******************************************************************************/
static void _init_gpio() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DUT_NSS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DUT_NSS_GPIO_Port, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(GPIO_NSS_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(GPIO_NSS_IRQ);
}
/******************************************************************************/
error_t commit_dut_spi() {
	SPI_HandleTypeDef *hspi = &dut_spi.hspi;
	TIM_HandleTypeDef *htmr = &(dut_spi.htmr);
	TIM_IC_InitTypeDef* ctmr_ic = &(dut_spi.ctmr_ic);
	spi_t *reg = dut_spi.reg;

	if (reg->mode.init) {
		return 0;
	}
	__HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE | SPI_CR2_ERRIE | SPI_CR2_TXEIE);

	hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
	if (reg->mode.cpha) {
		hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
	}

	hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	if (reg->mode.cpol) {
		hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
	}
	reg->frame_ticks = 0;
	reg->prev_ticks = 0;
	reg->w_count = 0;
	reg->r_count = 0;
	reg->transfer_count = 0;
	dut_spi.initial_byte = SPI_NO_DATA_BYTE;
	memset(dut_spi.reg->sm_buf, 0, sizeof(dut_spi.reg->sm_buf));
	memset(dut_spi.buf, 0, sizeof(dut_spi.buf));
	dut_spi.poll_index = 0;

	if (reg->mode.if_type == SPI_IF_TYPE_REG) {
		dut_spi.if_mode_int = _spi_reg_int;
	} else if (reg->mode.if_type == SPI_IF_TYPE_HS) {
		dut_spi.if_mode_int = _spi_hs_int;
	} else if (reg->mode.if_type == SPI_IF_TYPE_ECHO) {
		dut_spi.if_mode_int = _spi_echo_int;
	} else if (reg->mode.if_type == SPI_IF_TYPE_SM) {
		dut_spi.if_mode_int = NULL;
		dut_spi.sm_active = 1;
	} else {
		read_reg(0, &(dut_spi.initial_byte));
		dut_spi.if_mode_int = _spi_const_int;
	}

	reg->mode.init = 1;

	__HAL_SPI_DISABLE(hspi);
	if (!reg->mode.disable) {
		if(reg->mode.if_type == SPI_IF_TYPE_SM){
			HAL_SPI_DeInit(hspi);
			_init_periph_spi_ic();
			if(hspi->Init.CLKPolarity == SPI_POLARITY_LOW) {
				ctmr_ic->ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
			}
			else {
				ctmr_ic->ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
			}

			if (HAL_TIM_IC_ConfigChannel(htmr, ctmr_ic, DUT_SPI_IC_CHANNEL) != HAL_OK) {
				_Error_Handler(__FILE__, __LINE__);
			}
			if (HAL_TIM_IC_Start_DMA(htmr, DUT_SPI_IC_CHANNEL, (uint32_t*)dut_spi.buf, sizeof(dut_spi.buf)/sizeof(dut_spi.buf[0])) != HAL_OK) {
				_Error_Handler(__FILE__, __LINE__);
			}
		}
		else {
			_deinit_periph_spi_ic(htmr);
			HAL_SPI_Init(hspi);
			__HAL_SPI_ENABLE(hspi);
			hspi->Instance->DR = dut_spi.initial_byte;
			__HAL_SPI_ENABLE_IT(hspi, SPI_IT_RXNE | SPI_CR2_ERRIE | SPI_CR2_TXEIE);
		}
	}
	else {
		HAL_SPI_DeInit(hspi);
		_deinit_periph_spi_ic(htmr);
		if (init_basic_gpio(reg->dut_miso, DUT_MISO) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_mosi, DUT_MOSI) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_nss, DUT_NSS) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_sck, DUT_SCK) != 0) {
			return EINVAL;
		}
	}
	return 0;
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void update_dut_spi_inputs() {
	dut_spi.reg->status.clk = HAL_GPIO_ReadPin(DUT_SCK);
	dut_spi.reg->dut_miso.level = HAL_GPIO_ReadPin(DUT_MISO);
	dut_spi.reg->dut_mosi.level = HAL_GPIO_ReadPin(DUT_MOSI);
	dut_spi.reg->dut_nss.level = HAL_GPIO_ReadPin(DUT_NSS);
	dut_spi.reg->dut_sck.level = HAL_GPIO_ReadPin(DUT_SCK);
}

/******************************************************************************/
/*           Interrupt Handling                                               */
/******************************************************************************/
/**
 * @brief This function handles DUT_SPI global interrupt.
 */
void DUT_SPI_INT(void) {
	dut_spi.if_mode_int();
}

/******************************************************************************/
static void _spi_reg_int() {
	SPI_HandleTypeDef *hspi = &(dut_spi.hspi);
	spi_t *spi = dut_spi.reg;

	if (spi->state == SPI_FRAME_STARTED) {
		if (hspi->Instance->SR & SPI_FLAG_RXNE) {
			uint32_t data = hspi->Instance->DR;
			unprotected_read_uint8(data & (~SPI_ADDR_MASK),
					(uint8_t*) &(hspi->Instance->DR));
			if (data & SPI_ADDR_MASK) {
				spi->state = SPI_READING;
			} else {
				spi->state = SPI_WRITING;
			}
			spi->start_reg_index = data & (~SPI_ADDR_MASK);
			spi->reg_index = spi->start_reg_index;
			spi->transfer_count++;
		}
	} else if (spi->state == SPI_READING || spi->state == SPI_WRITING) {
		uint32_t itflag = hspi->Instance->SR;
		if (itflag & SPI_FLAG_TXE) {
			unprotected_read_uint8(spi->reg_index + 1,
					(uint8_t*) &(hspi->Instance->DR));
		}
		if (itflag & SPI_FLAG_RXNE) {
			uint8_t data = hspi->Instance->DR;
			if (spi->state == SPI_WRITING) {
				if (write_reg(spi->reg_index, data, PERIPH_ACCESS)) {
					spi->status.index_err = 1;
				}
				spi->w_count++;
			} else {
				spi->r_count++;
			}
			add_index(&dut_spi.reg->reg_index);
			spi->transfer_count++;
		}
	}
}

static void _spi_hs_int() {
	SPI_HandleTypeDef *hspi = &(dut_spi.hspi);
	spi_t *spi = dut_spi.reg;
	uint32_t itflag = hspi->Instance->SR;

	if (itflag & SPI_FLAG_TXE) {
		unprotected_read_uint8(spi->reg_index,
				(uint8_t*) &(hspi->Instance->DR));
		dut_spi.reg->state = SPI_TRANSFERING;
	}
	if (itflag & SPI_FLAG_RXNE) {
		if (write_reg(spi->reg_index, (uint8_t) (hspi->Instance->DR),
		PERIPH_ACCESS)) {
			spi->status.index_err = 1;
		}
		add_index(&dut_spi.reg->reg_index);
		spi->transfer_count++;
		// get ticks per byte
		dut_spi.reg->state = SPI_TRANSFERING;
	}

}

static void _spi_echo_int() {
	if (dut_spi.hspi.Instance->SR & SPI_FLAG_RXNE) {
		uint32_t tmp = dut_spi.hspi.Instance->DR;
		dut_spi.hspi.Instance->DR = tmp;
		dut_spi.reg->transfer_count++;
	}
}

#pragma GCC push_options
#pragma GCC optimize ("O3")
static void _spi_const_int() {
	if (dut_spi.hspi.Instance->SR & SPI_FLAG_RXNE) {
		(void)dut_spi.hspi.Instance->DR;
		dut_spi.reg->byte_ticks = get_tick32() - dut_spi.reg->prev_ticks;
		dut_spi.reg->prev_ticks += dut_spi.reg->byte_ticks;
		dut_spi.reg->transfer_count++;
	}
}
#pragma GCC pop_options

/**
 * @brief Interrupt for the spi line raise or lower
 */
#pragma GCC push_options
#pragma GCC optimize ("O3")
void GPIO_NSS_INT() {
	if (HAL_GPIO_ReadPin(DUT_NSS)) {
		/* Finished frame */
		uint32_t itflag = dut_spi.hspi.Instance->SR;

		dut_spi.reg->frame_ticks = get_tick32() - dut_spi.reg->frame_ticks;
		dut_spi.reg->status.end_clk = HAL_GPIO_ReadPin(DUT_SCK);

		/* read error flags */
		memset(&dut_spi.reg->status, 0, sizeof(dut_spi.reg->status));
		if (itflag & SPI_SR_MODF) {
			dut_spi.reg->status.modf = 1;
		}
		if (itflag & SPI_SR_BSY) {
			dut_spi.reg->status.bsy = 1;
		}
		if (itflag & SPI_SR_OVR) {
			dut_spi.reg->status.ovr = 1;
		}
		if (itflag & SPI_SR_UDR) {
			dut_spi.reg->status.udr = 1;
		}

		/* clear status and errors */
		uint32_t tmp = dut_spi.hspi.Instance->DR;
		(void) tmp;
		dut_spi.hspi.Instance->DR = dut_spi.initial_byte;

		dut_spi.reg->state = SPI_FRAME_FINISHED;
	} else {
		/* Starting frame */
		if (dut_spi.reg->mode.if_type == SPI_IF_TYPE_CONST) {
			dut_spi.reg->transfer_count = 0;
			dut_spi.reg->frame_ticks = get_tick32();
			dut_spi.reg->prev_ticks = dut_spi.reg->frame_ticks;
			if (dut_spi.hspi.Instance->SR & SPI_FLAG_RXNE) {
				(void)dut_spi.hspi.Instance->DR;
				dut_spi.reg->transfer_count = 1;
			}
			return;
		}
		dut_spi.reg->status.start_clk = HAL_GPIO_ReadPin(DUT_SCK);
		dut_spi.hspi.Instance->DR = dut_spi.initial_byte;
		dut_spi.reg->transfer_count = 0;
		if (dut_spi.reg->mode.if_type == SPI_IF_TYPE_ECHO) {
			return;
		}
		dut_spi.reg->reg_index = 0;
		dut_spi.reg->state = SPI_FRAME_STARTED;
	}
}
#pragma GCC pop_options

/******************************************************************************/
/*           Clock speed measurement                                          */
/******************************************************************************/
void poll_dut_spi_ic(){
	if (dut_spi.sm_active){
		uint16_t current_index = sizeof(dut_spi.buf)/sizeof(dut_spi.buf[0]) - TIMER_REMAINING_BUF(dut_spi.htmr_dma);
		while (dut_spi.poll_index < current_index){
			dut_spi.reg->sm_buf[dut_spi.poll_index] = dut_spi.buf[dut_spi.poll_index];
			dut_spi.poll_index++;
			dut_spi.reg->transfer_count = (int)(dut_spi.poll_index);
		}
	}
}

/**
 * @brief This function handles spi_ic_dma event interrupt.
 */
void DUT_SPI_IC_DMA_INT(void) {
	HAL_DMA_IRQHandler(&dut_spi.htmr_dma);
}
