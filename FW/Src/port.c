/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/*
 ******************************************************************************
 * @file			port.c
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Board/cpu specific porting.
 ******************************************************************************
 */
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"

#include "app_common.h"

#include "adc.h"
#include "tmr.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"
#include "pwm_dac.h"

#include "port.h"

extern void _Error_Handler(char *, int);

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
	__HAL_RCC_AFIO_CLK_ENABLE();
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* MemoryManagement_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	/* BusFault_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	/* UsageFault_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	/* SVCall_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	/* DebugMonitor_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, DEFAULT_INT_PRIO, 0);

	__HAL_AFIO_REMAP_SWJ_NOJTAG();
}

void init_clock(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/* Initializes the CPU, AHB and APB buses clocks */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_STATE;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/*Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/* Configure the Systick interrupt time */
	HAL_SYSTICK_Config((1UL) << TICK_BIT_OFFSET);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, DEFAULT_INT_PRIO, 0);
}
/******************************************************************************/
/*           MSP Calls                                                        */
/******************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == DUT_ADC_INST) {
		init_dut_adc_msp();
	}

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == DUT_ADC_INST) {
		deinit_dut_adc_msp();
	}
}

/******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
	if (htim_base->Instance == DUT_IC_INST) {
		init_dut_ic_msp();
	}
	else if (htim_base->Instance == DUT_PWM_DAC_INST) {
		init_dut_pwm_dac_msp();
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) {
	if (htim_base->Instance == DUT_IC_INST) {
		deinit_dut_ic_msp();
	}
	else if (htim_base->Instance == DUT_PWM_DAC_INST) {
		deinit_dut_pwm_dac_msp();
	}
}

/******************************************************************************/
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == DUT_IC_INST) {
			init_dut_ic_msp();
	}
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == DUT_IC_INST) {
		deinit_dut_ic_msp();
	}
}

/******************************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == DUT_PWM_DAC_INST) {
		init_dut_pwm_dac_msp();
	}
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == DUT_PWM_DAC_INST) {
		postinit_dut_pwm_dac_msp();
	}
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim) {
	if (htim->Instance == DUT_PWM_DAC_INST) {
		deinit_dut_pwm_dac_msp();
	}
}

/******************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
	if (huart->Instance == IF_UART_INST) {
		init_if_uart_msp();
	}
	else if (huart->Instance == DUT_UART_INST) {
		init_dut_uart_msp();
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
	if (huart->Instance == IF_UART_INST) {
		deinit_if_uart_msp();
	}
	else if (huart->Instance == DUT_UART_INST) {
		deinit_dut_uart_msp();
	}
}

/******************************************************************************/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
	if (hi2c->Instance == DUT_I2C_INST) {
		init_dut_i2c_msp();
	}

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c) {
	if (hi2c->Instance == DUT_I2C_INST) {
		deinit_dut_i2c_msp();
	}

}

/******************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
	if (hspi->Instance == DUT_SPI_INST) {
		init_dut_spi_msp();
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
	if (hspi->Instance == DUT_SPI_INST) {
		deinit_dut_spi_msp();
	}
}

/******************************************************************************/
/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 */
void _Error_Handler(char *file, int line) {
	while (1) {

	}
}

void init_periphs() {
	/* Prevents i2c clk from toggling at init */
	__HAL_RCC_I2C1_CLK_ENABLE();

	__HAL_RCC_DMA1_CLK_ENABLE();
}
