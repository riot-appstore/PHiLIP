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
 * @file			tmr.c
 * @author			Kevin Weiss
 * @date			03.03.2020
 * @brief			Controls the timer peripheral.
 *
 * This module requires very low jitter since the precision is 14ns. In
 * interrupt mode events can occur as fast as 1us apart, however, time must be
 * allowed for the buffer to be processed. With the DMA it appears that events
 * can still be caught up to 200ns. However the buffer still must not overflow
 * too quickly
 *
 * @note  Testing shows the a 150us wait must happen for every 255 events to
 * prevent any data corruption.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "PHiLIP_defaults.h"
#include "port.h"
#include "gpio.h"
#include "trace.h"

#include "tmr.h"

/* Private enums/structs ******************************************************/
/** @brief					The parameters for tmr control */
typedef struct {
	TIM_HandleTypeDef htmr; /**< Handle for the tmr device */
	DMA_HandleTypeDef htmr_dma; /**< Handle for the tmr dma */
	TIM_IC_InitTypeDef ctmr_ic; /**< Initialization for timer */
	tmr_t *reg; /**< Timer related registers */
	uint32_t of_tick; /**< The overflow tick counter */
	uint16_t buf[256]; /**< Buffer for DMA or interrupt ticks */
	uint8_t int_index; /**< Interrupt buffer index */
	uint8_t poll_index; /**< Buffer index when polling */
	uint16_t initial_level; /**< When measuing both edges this is the initial value */
	tmr_mode_t mode; /**< Internal timer mode settings */
} tmr_dev_t;

static tmr_dev_t dut_ic = {0}; /**< DUT_IC device */
/** @} */

/* Private function prototypes ************************************************/
static void _init_periph_ic();
static uint8_t _get_current_index();
static void _flush_tmr_buf(uint8_t dma_index, uint32_t of_tick);

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_dut_ic(map_t *map) {
	dut_ic.reg = &(map->tmr);
	_init_periph_ic();
	commit_dut_ic();
}

/******************************************************************************/
static void _init_periph_ic() {
	TIM_HandleTypeDef *htmr = &(dut_ic.htmr);
	TIM_IC_InitTypeDef* ctmr_ic = &(dut_ic.ctmr_ic);
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htmr->Instance = DUT_IC_INST;
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

void init_dut_ic_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_HandleTypeDef *htmr = &(dut_ic.htmr);
	DMA_HandleTypeDef *htmr_dma = &(dut_ic.htmr_dma);
	/* Peripheral clock enable */
	DUT_IC_CLK_EN();
	DUT_IC_GPIO_CLK_EN();

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_IC_Pin;
	if (dut_ic.reg->mode.trig_edge == DUT_IC_EDGE_BOTH) {
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	}
	else {
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DUT_IC_GPIO_Port, &GPIO_InitStruct);

	htmr_dma->Instance = DUT_IC_DMA_CHANNEL;
	htmr_dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
	htmr_dma->Init.PeriphInc = DMA_PINC_DISABLE;
	htmr_dma->Init.MemInc = DMA_MINC_ENABLE;
	htmr_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	htmr_dma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	htmr_dma->Init.Mode = DMA_CIRCULAR;
	htmr_dma->Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(htmr_dma) != HAL_OK)
    {
    	_Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(htmr,hdma[DUT_IC_DMA_ID],*htmr_dma);
    /* Priority must be high to ensure no jitter
     * Since the only data changing is the index and buffer and it is protected
     * we can use different priorities without worrying about */
    HAL_NVIC_SetPriority(DUT_IC_GPIO_IRQ, DEFAULT_INT_PRIO - 1, 0);
    HAL_NVIC_EnableIRQ(DUT_IC_GPIO_IRQ);
    HAL_NVIC_SetPriority(DUT_IC_DMA_IRQ, DEFAULT_INT_PRIO, 0);
    HAL_NVIC_EnableIRQ(DUT_IC_DMA_IRQ);
}

void deinit_dut_ic_msp() {
	TIM_HandleTypeDef *htmr = &(dut_ic.htmr);

	DUT_IC_CLK_DIS();
	HAL_GPIO_DeInit(DUT_IC_GPIO_Port, DUT_IC_Pin);
	HAL_DMA_DeInit(htmr->hdma[DUT_IC_DMA_ID]);

    HAL_NVIC_DisableIRQ(DUT_IC_DMA_IRQ);
    HAL_NVIC_DisableIRQ(DUT_IC_GPIO_IRQ);
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
error_t commit_dut_ic() {
	TIM_HandleTypeDef *htmr = &(dut_ic.htmr);
	TIM_IC_InitTypeDef* ctmr_ic = &(dut_ic.ctmr_ic);
	tmr_t *reg = dut_ic.reg;

	if (reg->mode.init) {
		return 0;
	}

	HAL_TIM_IC_Stop_DMA(htmr, DUT_IC_CHANNEL);
	dut_ic.mode.disable = reg->mode.disable;
	if (dut_ic.reg->mode.disable) {
		if (HAL_TIM_Base_DeInit(htmr) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}
		if (init_basic_gpio(reg->dut_ic, DUT_IC) != 0) {
			return -EINVAL;
		}
		return 0;
	}

	if (HAL_TIM_Base_Init(htmr) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	dut_ic.poll_index = 0;
	dut_ic.int_index = 0;
	dut_ic.of_tick = 0;
	dut_ic.mode.trig_edge = reg->mode.trig_edge;
	reg->max_tick = 0;
	reg->min_tick = DEFAULT_TMR_MIN_TICK;
	reg->min_holdoff = DMA_HOLDOFF_NS;
	if (reg->mode.trig_edge == DUT_IC_EDGE_BOTH) {
		reg->min_holdoff = INT_HOLDOFF_NS;
		if (HAL_TIM_Base_Start(htmr) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}
		if (HAL_GPIO_ReadPin(DUT_IC)) {
			dut_ic.initial_level = EVENT_TYPE_FALLING;
		}
		else {
			dut_ic.initial_level = EVENT_TYPE_RISING;
		}
		return 0;
	}
	else if (reg->mode.trig_edge == DUT_IC_EDGE_RISING) {
		ctmr_ic->ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	}
	else {
		ctmr_ic->ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	}

	if (HAL_TIM_IC_ConfigChannel(htmr, ctmr_ic, DUT_IC_CHANNEL) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_IC_Start_DMA(htmr, DUT_IC_CHANNEL, (uint32_t*)dut_ic.buf, sizeof(dut_ic.buf)/sizeof(dut_ic.buf[0])) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	HAL_NVIC_DisableIRQ(DUT_IC_GPIO_IRQ);
	return 0;
}

/******************************************************************************/
void poll_dut_ic() {
	TIM_HandleTypeDef *htmr = &(dut_ic.htmr);
	uint32_t of_tick = 0;
	uint8_t dma_index;

	dma_index = _get_current_index();

	if (DUT_IC_OV_OCCURED(htmr)) {
		DUT_IC_OV_CLEAR(htmr);
		of_tick = OVERFLOW_TICK_VAL;
	}

	_flush_tmr_buf(dma_index, of_tick);

	if (of_tick){

		dma_index = _get_current_index();
		_flush_tmr_buf(dma_index, of_tick);
	}
	dut_ic.of_tick += of_tick;
}

static uint8_t _get_current_index() {
	uint8_t dma_index;

	if (dut_ic.mode.trig_edge == DUT_IC_EDGE_BOTH) {
		do {
			dma_index = dut_ic.int_index;
		} while (dma_index != dut_ic.int_index);
	}
	else {
		uint32_t dma_bytes_to_go;
		do {
			dma_bytes_to_go = DUT_IC_REMAINING_BUF(dut_ic.htmr_dma);
		} while (dma_bytes_to_go != DUT_IC_REMAINING_BUF(dut_ic.htmr_dma));
		dma_index = sizeof(dut_ic.buf)/sizeof(dut_ic.buf[0]) - dma_bytes_to_go;
	}

	return dma_index;
}

static void _flush_tmr_buf(uint8_t dma_index, uint32_t of_tick) {
	tmr_t *reg = dut_ic.reg;
	uint16_t edge = EVENT_TYPE_FALLING;
	uint32_t tick = 0;
	static uint32_t old_tick = 0;

	if (dut_ic.mode.trig_edge == DUT_IC_EDGE_RISING) {
		edge = EVENT_TYPE_RISING;
	}
	while (dut_ic.poll_index != dma_index) {
		if (dut_ic.mode.trig_edge == DUT_IC_EDGE_BOTH) {
			edge = dut_ic.initial_level;
			if (dut_ic.initial_level == EVENT_TYPE_FALLING) {
				dut_ic.initial_level = EVENT_TYPE_RISING;
			}
			else {
				dut_ic.initial_level = EVENT_TYPE_FALLING;
			}
		}
		if (dut_ic.buf[dut_ic.poll_index] < dut_ic.htmr.Instance->CNT) {
			tick = dut_ic.of_tick + dut_ic.buf[dut_ic.poll_index] + of_tick;
		}
		else {
			tick = dut_ic.of_tick + dut_ic.buf[dut_ic.poll_index];
		}
		store_tick_from_buf(SOURCE_DUT_IC, edge, tick);
		if (tick > old_tick && dut_ic.poll_index > 0) {
			uint32_t diff = tick - old_tick;
			if (reg->max_tick < diff) {
				reg->max_tick = diff;
			}
			if (reg->min_tick > diff) {
				reg->min_tick = diff;
			}
		}
		old_tick = tick;
		dut_ic.poll_index++;
	}
}

void update_dut_ic_inputs() {
	dut_ic.reg->dut_ic.level = HAL_GPIO_ReadPin(DUT_IC);
}
/******************************************************************************/
/*           Interrupt Handling                                               */
/******************************************************************************/
/**
 * @brief This function handles dut_ic dma event interrupt.
 */
void DUT_IC_DMA_INT() {
	HAL_DMA_IRQHandler(&dut_ic.htmr_dma);
}

/**
 * @brief Trigger on edge, must be quick and save counter value in buffer.
 */
void DUT_IC_GPIO_INT() {
	EXTI->PR = DUT_IC_Pin;
	dut_ic.buf[dut_ic.int_index++] = dut_ic.htmr.Instance->CNT;
}
