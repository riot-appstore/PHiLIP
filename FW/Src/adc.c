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
 * @file			adc.c
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Controls the adc peripheral.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "mm_typedefs.h"
#include "port.h"
#include "gpio.h"

#include "adc.h"

/* Private enums/structs ******************************************************/
/** @brief					The parameters for adc control */
typedef struct {
	ADC_HandleTypeDef hadc; /**< Handle for the adc device */
	adc_t *reg; /**< adc application registers */
	adc_mode_t mode; /**< current mode settings */
	uint32_t num_of_samples; /**< current number of adc samples in sum */

} adc_dev_t;
/** @} */

/* Private function prototypes ************************************************/
static void _init_periph_adc();
static void _init_dut_adc_reg(map_t *map);

/* Private variables **********************************************************/
static adc_dev_t dut_adc; /**< DUT ADC module instance */

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_dut_adc(map_t *map) {
	_init_periph_adc();
	_init_dut_adc_reg(map);
	commit_dut_adc();
}

/******************************************************************************/
static void _init_periph_adc() {
	ADC_HandleTypeDef *hadc = &(dut_adc.hadc);

	hadc->Instance = DUT_ADC_INST;

	hadc->Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc->Init.ContinuousConvMode = ENABLE;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(hadc) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
}

void init_dut_adc_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* Peripheral clock enable */
	DUT_ADC_CLK_EN();
	DUT_ADC_GPIO_CLK_EN();

	GPIO_InitStruct.Pin = DUT_ADC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(DUT_ADC_GPIO_Port, &GPIO_InitStruct);

#ifdef DUT_ADC_ENABLE_INT
	HAL_NVIC_SetPriority(ADC_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQ);
#endif
}

void deinit_dut_adc_msp() {
	DUT_ADC_CLK_DIS();
	HAL_GPIO_DeInit(DUT_ADC);
}

/******************************************************************************/
static void _init_dut_adc_reg(map_t *map) {
	dut_adc.reg = &(map->adc);
}

/******************************************************************************/
error_t commit_dut_adc() {
	ADC_HandleTypeDef *hadc = &(dut_adc.hadc);
	adc_t *reg = dut_adc.reg;
	ADC_ChannelConfTypeDef sConfig;

	static int calibrate_adc = 1;

	if (reg->mode.init) {
		return 0;
	}

	HAL_ADC_Stop(hadc);
	dut_adc.mode.disable = reg->mode.disable;
	if (dut_adc.reg->mode.disable) {
		if (HAL_ADC_DeInit(hadc) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}
		if (init_basic_gpio(reg->dut_adc, DUT_ADC) != 0) {
			return -EINVAL;
		}
		return 0;
	}
	if (HAL_ADC_Init(hadc) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	if (calibrate_adc) {
		/* Calibration seems to add a 50mV offset and doesn't keep a stable
		 * initial value.
		 */
		while (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK);
		calibrate_adc = 0;
	}

	sConfig.Channel = ADC_CHANNEL;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	if (reg->mode.fast_sample) {
		sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	}
	else {
		sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	}
	if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}



	reg->mode.init = 1;
	reg->current_sum = 0;
	reg->index = 0;
	reg->sum = 0;
	dut_adc.num_of_samples = reg->num_of_samples;

	HAL_ADC_Start(hadc);

	return 0;
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void poll_dut_adc() {
	adc_t *reg = dut_adc.reg;
	if (dut_adc.mode.disable) {
		dut_adc.reg->dut_adc.level = HAL_GPIO_ReadPin(DUT_ADC);
	}
	else if (ADC_DATA_READY(dut_adc.hadc)) {
		uint32_t val = HAL_ADC_GetValue(&dut_adc.hadc);
		reg->sample = val;
		reg->current_sum += val;
		reg->index++;
		if (reg->index >= dut_adc.num_of_samples) {
			reg->sum = reg->current_sum;
			reg->index = 0;
			reg->current_sum = 0;
			reg->counter++;
		}
	}
}

/******************************************************************************/
/*           Interrupt Handling                                               */
/******************************************************************************/
#ifdef DUT_ADC_ENABLE_INT
/**
 * @brief This function handles an ADC interrupt
 */
void ADC_INT() {

}
#endif
