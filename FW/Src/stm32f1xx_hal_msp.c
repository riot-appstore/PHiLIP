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

#include "port.h"

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
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(DUT_NSS_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_SCK_Pin | DUT_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_MISO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DUT_MISO_GPIO_Port, &GPIO_InitStruct);

		__HAL_AFIO_REMAP_SPI1_ENABLE()
		;

		/* SPI1 interrupt Init */
		HAL_NVIC_SetPriority(SPI1_IRQn, DEFAULT_INT_PRIO, 0);
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
		GPIO_InitStruct.Pin = DUT_NSS_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(DUT_NSS_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_SCK_Pin | DUT_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DUT_MISO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		HAL_GPIO_Init(DUT_MISO_GPIO_Port, &GPIO_InitStruct);

		/* SPI2 interrupt Init */
		HAL_NVIC_SetPriority(SPI2_IRQn, DEFAULT_INT_PRIO, 0);
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

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
