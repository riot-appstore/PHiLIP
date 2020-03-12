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
 * @file			pwm_dac.c
 * @author			Kevin Weiss
 * @date			15.03.2019
 * @brief			Controls the pwm and dac peripheral.
 * @note			The PWM and DAC share the same timer so changing the period
 * 					affects both.
 ******************************************************************************
 */
/* Includes *******************************************************************/
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_common.h"

#include "gpio.h"

#include "pwm_dac.h"

/* Private enums/structs ******************************************************/
/** @brief					The parameters for pwm control */
typedef struct {
	TIM_HandleTypeDef *htmr; /**< Handle of timer */
	TIM_OC_InitTypeDef hoc; /**< Handle of output */
	pwm_t *reg; /**< pwm live application registers */
} pwm_dev;

/** @brief					The parameters for dac control */
typedef struct {
	TIM_HandleTypeDef *htmr; /**< Handle of timer */
	TIM_OC_InitTypeDef hoc; /**< Handle of output */
	dac_t *reg; /**< dac live application registers */
} dac_dev;
/** @} */

/* Private variables **********************************************************/
static TIM_HandleTypeDef htmr;

static pwm_dev dut_pwm;
static dac_dev dut_dac;

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_dut_pwm_dac(map_t *reg) {
	dut_pwm.htmr = &htmr;
	dut_pwm.reg = &(reg->pwm);

	dut_dac.htmr = &htmr;
	dut_dac.reg = &(reg->dac);

	DUT_PWM_DAC_CLK_EN();

	dut_pwm.htmr->Instance = DUT_PWM_TMR;
	dut_pwm.htmr->Init.CounterMode = TIM_COUNTERMODE_UP;
	dut_pwm.htmr->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	dut_pwm.htmr->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	commit_dut_pwm();
	commit_dut_dac();
}

/******************************************************************************/
void init_dut_pwm_dac_msp() {
	DUT_PWM_DAC_CLK_EN();
}

void postinit_dut_pwm_dac_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DUT_PWM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DUT_PWM_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_DAC_Pin;
	HAL_GPIO_Init(DUT_DAC_GPIO_Port, &GPIO_InitStruct);

	DUT_PWM_DAC_GPIO_AF_REMAP();
}

void deinit_dut_pwm_dac_msp() {
	DUT_PWM_DAC_CLK_DIS();

	HAL_GPIO_DeInit(DUT_PWM);
	HAL_GPIO_DeInit(DUT_DAC);
}

/******************************************************************************/
error_t commit_dut_pwm() {

	if (dut_pwm.reg->mode.init) {
		return 0;
	}

	HAL_TIM_PWM_Stop(dut_pwm.htmr, TIM_CHANNEL_3);

	if (dut_pwm.reg->mode.disable) {
		dut_pwm.hoc.OCMode = TIM_OCMODE_INACTIVE;
		HAL_GPIO_DeInit(DUT_PWM);
		if (init_basic_gpio(dut_pwm.reg->dut_pwm, DUT_PWM) != 0) {
			return EINVAL;
		}
	}
	else {
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = DUT_PWM_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(DUT_PWM_GPIO_Port, &GPIO_InitStruct);

		dut_pwm.reg->period = dut_pwm.reg->h_ticks + dut_pwm.reg->l_ticks;
		uint16_t div;
		dut_pwm.hoc.OCMode = TIM_OCMODE_PWM1;
		if ((dut_pwm.reg->h_ticks > dut_pwm.reg->period) ||
			(dut_pwm.reg->l_ticks > dut_pwm.reg->period)) {
			dut_pwm.reg->mode.init = 1;
			return EOVERFLOW;
		}
		for (div = 0; (dut_pwm.reg->period >> div) >= (uint32_t)0xFFFF; div++);
		dut_pwm.htmr->Init.Period = (dut_pwm.reg->period >> div);
		dut_pwm.htmr->Init.Prescaler = (1 << div) - 1;
		dut_pwm.hoc.Pulse = dut_pwm.reg->h_ticks>>div;
		if (dut_pwm.reg->period != 0) {
			dut_pwm.reg->duty_cycle = ((uint64_t)dut_pwm.reg->h_ticks * 10000) / dut_pwm.reg->period;
		}
	}
	dut_pwm.reg->mode.init = 1;

	TIM_Base_SetConfig(dut_pwm.htmr->Instance, &dut_pwm.htmr->Init);
	HAL_TIM_PWM_ConfigChannel(dut_pwm.htmr, &dut_pwm.hoc, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(dut_pwm.htmr, TIM_CHANNEL_3);

	return 0;
}

/******************************************************************************/
error_t commit_dut_dac() {
	dac_t *reg = dut_dac.reg;
	TIM_HandleTypeDef *htmr = dut_dac.htmr;

	if (reg->mode.init) {
		return 0;
	}

	if (reg->level > 10000) {
		return EINVAL;
	}
	if (reg->mode.disable) {
		HAL_GPIO_DeInit(DUT_DAC);
		if (init_basic_gpio(dut_dac.reg->dut_dac, DUT_DAC) != 0) {
			return EINVAL;
		}
		dut_dac.hoc.OCMode = TIM_OCMODE_INACTIVE;
	}
	else {
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = DUT_DAC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(DUT_DAC_GPIO_Port, &GPIO_InitStruct);
		dut_dac.hoc.OCMode = TIM_OCMODE_PWM1;
	}

	dut_dac.hoc.Pulse = ((uint32_t)reg->level * htmr->Init.Period) / 10000;
	HAL_TIM_PWM_ConfigChannel(htmr, &dut_dac.hoc, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(htmr, TIM_CHANNEL_4);
	return 0;
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void update_dut_pwm_inputs() {
	dut_pwm.reg->dut_pwm.level = HAL_GPIO_ReadPin(DUT_PWM);
}

void update_dut_dac_inputs() {
	dut_dac.reg->dut_dac.level = HAL_GPIO_ReadPin(DUT_DAC);
}
