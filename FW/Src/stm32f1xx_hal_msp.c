/**
 ******************************************************************************
 * File Name          : stm32f1xx_hal_msp.c
 * Description        : This file provides code for the MSP Initialization
 *                      and de-Initialization codes.
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

extern DMA_HandleTypeDef hdma_adc_pm;
extern DMA_HandleTypeDef hdma_usart_if_rx;
extern DMA_HandleTypeDef hdma_usart_if_tx;
extern DMA_HandleTypeDef hdma_usart_dut_rx;
extern DMA_HandleTypeDef hdma_usart_dut_tx;

extern void _Error_Handler(char *, int);
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
	/* USER CODE BEGIN MspInit 0 */

	/* USER CODE END MspInit 0 */

	__HAL_RCC_AFIO_CLK_ENABLE();

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* System interrupt init*/
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
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	/**NOJTAG: JTAG-DP Disabled and SW-DP Enabled
	 */
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hadc->Instance == ADC1) {
		/* Peripheral clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();

#ifdef BLUEPILL
		/**ADC1 GPIO Configuration
		PA7     ------> ADC1_IN7
		PB0     ------> ADC1_IN8
		PB1     ------> ADC1_IN9
		*/
		GPIO_InitStruct.Pin = PM_LO_ADC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(PM_LO_ADC_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = PM_HI_ADC_Pin|PM_V_ADC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
#ifdef NUCLEOF103RB
	    /**ADC1 GPIO Configuration
	    PC0     ------> ADC1_IN10
	    PC1     ------> ADC1_IN11
	    PC2     ------> ADC1_IN12
	    */
	    GPIO_InitStruct.Pin = PM_V_ADC_Pin|PM_LO_ADC_Pin|PM_HI_ADC_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

		/* ADC1 DMA Init */
		/* ADC1 Init */
		hdma_adc_pm.Instance = DMA1_Channel1;
		hdma_adc_pm.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc_pm.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc_pm.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc_pm.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc_pm.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc_pm.Init.Mode = DMA_NORMAL;
		hdma_adc_pm.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_adc_pm) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc_pm);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
		/* USER CODE BEGIN ADC1_MspInit 1 */

		/* USER CODE END ADC1_MspInit 1 */
	} else if (hadc->Instance == ADC2) {
		/* USER CODE BEGIN ADC2_MspInit 0 */

		/* USER CODE END ADC2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_ADC2_CLK_ENABLE()
		;

		/**ADC2 GPIO Configuration
		 PA7     ------> ADC2_IN7
		 */
		GPIO_InitStruct.Pin = DUT_ADC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(DUT_ADC_GPIO_Port, &GPIO_InitStruct);

		/* ADC2 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
		/* USER CODE BEGIN ADC2_MspInit 1 */

		/* USER CODE END ADC2_MspInit 1 */
	}

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc) {

	if (hadc->Instance == ADC1) {
		/* USER CODE BEGIN ADC1_MspDeInit 0 */

		/* USER CODE END ADC1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_ADC1_CLK_DISABLE();

#ifdef BLUEPILL
	    /**ADC1 GPIO Configuration
	    PA7     ------> ADC1_IN7
	    PB0     ------> ADC1_IN8
	    PB1     ------> ADC1_IN9
	    */
	    HAL_GPIO_DeInit(PM_LO_ADC_GPIO_Port, PM_LO_ADC_Pin);

	    HAL_GPIO_DeInit(GPIOB, PM_HI_ADC_Pin|PM_V_ADC_Pin);
#endif
#ifdef NUCLEOF103RB
	    /**ADC1 GPIO Configuration
	    PC0     ------> ADC1_IN10
	    PC1     ------> ADC1_IN11
	    PC2     ------> ADC1_IN12
	    */
	    HAL_GPIO_DeInit(GPIOC, PM_V_ADC_Pin|PM_LO_ADC_Pin|PM_HI_ADC_Pin);
#endif

		/* ADC1 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);

		/* ADC1 interrupt DeInit */
		/* USER CODE BEGIN ADC1:ADC1_2_IRQn disable */
		/**
		 * Uncomment the line below to disable the "ADC1_2_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(ADC1_2_IRQn); */
		/* USER CODE END ADC1:ADC1_2_IRQn disable */

		/* USER CODE BEGIN ADC1_MspDeInit 1 */

		/* USER CODE END ADC1_MspDeInit 1 */
	} else if (hadc->Instance == ADC2) {
		/* USER CODE BEGIN ADC2_MspDeInit 0 */

		/* USER CODE END ADC2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_ADC2_CLK_DISABLE();

		/**ADC2 GPIO Configuration
		 PA7     ------> ADC2_IN7
		 */
		HAL_GPIO_DeInit(DUT_ADC_GPIO_Port, DUT_ADC_Pin);

		/* ADC2 interrupt DeInit */
		/* USER CODE BEGIN ADC2:ADC1_2_IRQn disable */
		/**
		 * Uncomment the line below to disable the "ADC1_2_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(ADC1_2_IRQn); */
		/* USER CODE END ADC2:ADC1_2_IRQn disable */

		/* USER CODE BEGIN ADC2_MspDeInit 1 */

		/* USER CODE END ADC2_MspDeInit 1 */
	}

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hi2c->Instance == I2C1) {
#ifdef NUCLEOF103RB
		__HAL_AFIO_REMAP_I2C1_ENABLE();
#endif
		/**I2C1 GPIO Configuration
		 PB6     ------> I2C1_SCL
		 PB7     ------> I2C1_SDA
		 */
		GPIO_InitStruct.Pin = DUT_SCL_Pin | DUT_SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE()
		;
		/* I2C1 interrupt Init */
		HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
		HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
		/* USER CODE BEGIN I2C1_MspInit 1 */

		/* USER CODE END I2C1_MspInit 1 */
	}

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c) {

	if (hi2c->Instance == I2C1) {
		/* USER CODE BEGIN I2C1_MspDeInit 0 */

		/* USER CODE END I2C1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_I2C1_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		 PB6     ------> I2C1_SCL
		 PB7     ------> I2C1_SDA
		 */
		HAL_GPIO_DeInit(GPIOB, DUT_SCL_Pin | DUT_SDA_Pin);

		/* I2C1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
		HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
		/* USER CODE BEGIN I2C1_MspDeInit 1 */

		/* USER CODE END I2C1_MspDeInit 1 */
	}

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc) {

	if (hrtc->Instance == RTC) {
		/* USER CODE BEGIN RTC_MspInit 0 */

		/* USER CODE END RTC_MspInit 0 */
		HAL_PWR_EnableBkUpAccess();
		/* Enable BKP CLK enable for backup registers */
		__HAL_RCC_BKP_CLK_ENABLE()
		;
		/* Peripheral clock enable */
		__HAL_RCC_RTC_ENABLE();
		/* USER CODE BEGIN RTC_MspInit 1 */

		/* USER CODE END RTC_MspInit 1 */
	}

}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc) {

	if (hrtc->Instance == RTC) {
		/* USER CODE BEGIN RTC_MspDeInit 0 */

		/* USER CODE END RTC_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_RTC_DISABLE();
		/* USER CODE BEGIN RTC_MspDeInit 1 */

		/* USER CODE END RTC_MspDeInit 1 */
	}

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {

	GPIO_InitTypeDef GPIO_InitStruct;
#ifdef BLUEPILL
	if (hspi->Instance == SPI1) {
		/* Peripheral clock enable */
		__HAL_RCC_SPI1_CLK_ENABLE()
		;

		/**SPI1 GPIO Configuration
		 PA15     ------> SPI1_NSS
		 PB3     ------> SPI1_SCK
		 PB4     ------> SPI1_MISO
		 PB5     ------> SPI1_MOSI
		 */
		GPIO_InitStruct.Pin = DUT_NSS_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DUT_NSS_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_SCK_Pin | DUT_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_MISO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DUT_MISO_GPIO_Port, &GPIO_InitStruct);

		__HAL_AFIO_REMAP_SPI1_ENABLE()
		;

		/* SPI1 interrupt Init */
		HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(SPI1_IRQn);
	}
#endif
#ifdef NUCLEOF103RB
	if (hspi->Instance == SPI2) {
		/* Peripheral clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE()
		;

		/**SPI2 GPIO Configuration
		 PB12     ------> SPI2_NSS
		 PB13     ------> SPI2_SCK
		 PB14     ------> SPI2_MISO
		 PB15     ------> SPI2_MOSI
		 */
		GPIO_InitStruct.Pin = DUT_NSS_Pin | DUT_SCK_Pin | DUT_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_MISO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DUT_MISO_GPIO_Port, &GPIO_InitStruct);

		/* SPI2 interrupt Init */
		HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
#endif

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
#ifdef BLUEPILL
	if (hspi->Instance == SPI1) {
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		 PA15     ------> SPI1_NSS
		 PB3     ------> SPI1_SCK
		 PB4     ------> SPI1_MISO
		 PB5     ------> SPI1_MOSI
		 */
		HAL_GPIO_DeInit(DUT_NSS_GPIO_Port, DUT_NSS_Pin);

		HAL_GPIO_DeInit(GPIOB, DUT_SCK_Pin | DUT_MISO_Pin | DUT_MOSI_Pin);

		/* SPI1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(SPI1_IRQn);
	}

#endif
#ifdef NUCLEOF103RB
	if (hspi->Instance == SPI2) {

		/* Peripheral clock disable */
		__HAL_RCC_SPI2_CLK_DISABLE();

		/**SPI2 GPIO Configuration
		 PA15     ------> SPI1_NSS
		 PB3     ------> SPI1_SCK
		 PB4     ------> SPI1_MISO
		 PB5     ------> SPI1_MOSI
		 */

		HAL_GPIO_DeInit(GPIOB,
		DUT_NSS_Pin | DUT_SCK_Pin | DUT_MISO_Pin | DUT_MOSI_Pin);

		/* SPI2 interrupt DeInit */
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
	}
#endif
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (htim_base->Instance == TIM1) {
		/* USER CODE BEGIN TIM1_MspInit 0 */

		/* USER CODE END TIM1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM1_CLK_ENABLE()
		;

		/**TIM1 GPIO Configuration
		 PA8     ------> TIM1_CH1
		 */
		GPIO_InitStruct.Pin = DUT_IC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(DUT_IC_GPIO_Port, &GPIO_InitStruct);

		/* TIM1 interrupt Init */
		HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
		/* USER CODE BEGIN TIM1_MspInit 1 */

		/* USER CODE END TIM1_MspInit 1 */
	}
#ifdef BLUEPILL
	else if(htim_base->Instance==TIM4)
	{
		/* Peripheral clock enable */
		__HAL_RCC_TIM4_CLK_ENABLE();
	}
#endif
#ifdef NUCLEOF103RB
	else if (htim_base->Instance == TIM3) {
		/* Peripheral clock enable */
		__HAL_RCC_TIM3_CLK_ENABLE()
		;
	}
#endif
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (htim->Instance == TIM1) {
		/* USER CODE BEGIN TIM1_MspInit 0 */

		/* USER CODE END TIM1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM1_CLK_ENABLE()
		;

		/**TIM1 GPIO Configuration
		 PA8     ------> TIM1_CH1
		 */
		GPIO_InitStruct.Pin = DUT_IC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(DUT_IC_GPIO_Port, &GPIO_InitStruct);

		/* TIM1 interrupt Init */
		HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
		/* USER CODE BEGIN TIM1_MspInit 1 */

		/* USER CODE END TIM1_MspInit 1 */
	}

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim) {
#ifdef BLUEPILL
	if (htim->Instance == TIM4) {
		/* Peripheral clock enable */
		__HAL_RCC_TIM4_CLK_ENABLE();

	}
#endif
#ifdef NUCLEOF103RB
	if (htim->Instance == TIM3) {
		/* Peripheral clock enable */
		__HAL_RCC_TIM4_CLK_ENABLE();

	}
#endif
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) {

	GPIO_InitTypeDef GPIO_InitStruct;
#ifdef BLUEPILL
	if (htim->Instance == TIM4) {

		/**TIM4 GPIO Configuration
		 PB8     ------> TIM4_CH3
		 PB9     ------> TIM4_CH4
		 */
		GPIO_InitStruct.Pin = DUT_DAC_Pin | DUT_PWM_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	}
#endif
#ifdef NUCLEOF103RB
	if (htim->Instance == TIM3) {

		/**TIM3 GPIO Configuration
		 PC8     ------> TIM3_CH3
		 PC9     ------> TIM3_CH4
		 */
		GPIO_InitStruct.Pin = DUT_PWM_Pin | DUT_DAC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		__HAL_AFIO_REMAP_TIM3_ENABLE();

	}
#endif
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* htim) {

	if (htim->Instance == TIM1) {
		/* USER CODE BEGIN TIM1_MspDeInit 0 */

		/* USER CODE END TIM1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_TIM1_CLK_DISABLE();

		/**TIM1 GPIO Configuration
		 PA8     ------> TIM1_CH1
		 */
		HAL_GPIO_DeInit(DUT_IC_GPIO_Port, DUT_IC_Pin);

		/* TIM1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
		/* USER CODE BEGIN TIM1_MspDeInit 1 */

		/* USER CODE END TIM1_MspDeInit 1 */
	}

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim) {
#ifdef BLUEPILL
	if (htim->Instance == TIM4) {
		/* Peripheral clock disable */
		__HAL_RCC_TIM4_CLK_DISABLE();
	}
#endif
#ifdef NUCLEOF103RB
	if (htim->Instance == TIM4) {
		/* Peripheral clock disable */
		__HAL_RCC_TIM3_CLK_DISABLE();
	}
#endif
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart) {

	GPIO_InitTypeDef GPIO_InitStruct;
#ifdef BLUEPILL
	if (huart->Instance == USART1) {
		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE()
		;

		/**USART1 GPIO Configuration
		 PA9     ------> USART1_TX
		 PA10     ------> USART1_RX
		 */
		GPIO_InitStruct.Pin = IF_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(IF_TX_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = IF_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(IF_RX_GPIO_Port, &GPIO_InitStruct);

		/* USART1 DMA Init */
		/* USART1_RX Init */
		hdma_usart_if_rx.Instance = DMA1_Channel5;
		hdma_usart_if_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart_if_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart_if_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart_if_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart_if_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart_if_rx.Init.Mode = DMA_NORMAL;
		hdma_usart_if_rx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart_if_rx) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(huart, hdmarx, hdma_usart_if_rx);

		/* USART1_TX Init */
		hdma_usart_if_tx.Instance = DMA1_Channel4;
		hdma_usart_if_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart_if_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart_if_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart_if_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart_if_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart_if_tx.Init.Mode = DMA_NORMAL;
		hdma_usart_if_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart_if_tx) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(huart, hdmatx, hdma_usart_if_tx);

	} else if (huart->Instance == USART3) {
		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE()
		;

		/**USART3 GPIO Configuration
		 PB10     ------> USART3_TX
		 PB11     ------> USART3_RX
		 */
		GPIO_InitStruct.Pin = DUT_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USART3 DMA Init */
		/* USART3_RX Init */
		hdma_usart_dut_rx.Instance = DMA1_Channel3;
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

		/* USART3_TX Init */
		hdma_usart_dut_tx.Instance = DMA1_Channel2;
		hdma_usart_dut_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart_dut_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart_dut_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart_dut_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart_dut_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart_dut_tx.Init.Mode = DMA_NORMAL;
		hdma_usart_dut_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart_dut_tx) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(huart, hdmatx, hdma_usart_dut_tx);

		/* USART3 interrupt Init */
		HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
#endif
#ifdef NUCLEOF103RB
	if (huart->Instance == USART1) {
		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

	    /**USART1 GPIO Configuration
	    PA9     ------> USART1_TX
	    PA10     ------> USART1_RX
	    */
	    GPIO_InitStruct.Pin = DUT_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = DUT_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 DMA Init */
		/* USART1_RX Init */
		hdma_usart_dut_rx.Instance = DMA1_Channel5;
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

		/* USART1_TX Init */
		hdma_usart_dut_tx.Instance = DMA1_Channel4;
		hdma_usart_dut_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart_dut_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart_dut_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart_dut_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart_dut_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart_dut_tx.Init.Mode = DMA_NORMAL;
		hdma_usart_dut_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart_dut_tx) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(huart, hdmatx, hdma_usart_dut_tx);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	} else if (huart->Instance == USART2) {
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE()
		;

		/**USART2 GPIO Configuration
		 PA2     ------> USART2_TX
		 PA3     ------> USART2_RX
		 */
		GPIO_InitStruct.Pin = IF_TX_Pin | IF_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 DMA Init */
		/* USART2_RX Init */
		hdma_usart_if_rx.Instance = DMA1_Channel6;
		hdma_usart_if_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart_if_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart_if_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart_if_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart_if_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart_if_rx.Init.Mode = DMA_NORMAL;
		hdma_usart_if_rx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart_if_rx) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(huart, hdmarx, hdma_usart_if_rx);

		/* USART2_TX Init */
		hdma_usart_if_tx.Instance = DMA1_Channel7;
		hdma_usart_if_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart_if_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart_if_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart_if_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart_if_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart_if_tx.Init.Mode = DMA_NORMAL;
		hdma_usart_if_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart_if_tx) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		__HAL_LINKDMA(huart, hdmatx, hdma_usart_if_tx);
	}

#endif
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
#ifdef BLUEPILL
	if (huart->Instance == USART1) {
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
		 PA9     ------> USART1_TX
		 PA10     ------> USART1_RX
		 */
		HAL_GPIO_DeInit(GPIOA, IF_TX_Pin | IF_RX_Pin);

		/* USART1 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		HAL_DMA_DeInit(huart->hdmatx);
	} else if (huart->Instance == USART3) {
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration
		 PB10     ------> USART3_TX
		 PB11     ------> USART3_RX
		 PB13     ------> USART3_CTS
		 PB14     ------> USART3_RTS
		 */
		HAL_GPIO_DeInit(GPIOB,
				DUT_TX_Pin | DUT_RX_Pin);

		/* USART3 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		HAL_DMA_DeInit(huart->hdmatx);

		/* USART3 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART3_IRQn);
	}
#endif
#ifdef NUCLEOF103RB
	if (huart->Instance == USART1) {
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
		 PA9     ------> USART1_TX
		 PA10     ------> USART1_RX
		 */
		HAL_GPIO_DeInit(GPIOA, DUT_TX_Pin | DUT_RX_Pin);

		/* USART1 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		HAL_DMA_DeInit(huart->hdmatx);

		/* USART1 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART1_IRQn);
	} else if (huart->Instance == USART2) {
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		 PA2     ------> USART2_TX
		 PA3     ------> USART2_RX
		 */
		HAL_GPIO_DeInit(GPIOA, IF_TX_Pin | IF_RX_Pin);

		/* USART2 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		HAL_DMA_DeInit(huart->hdmatx);
	}
#endif

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
