/*
 * port.c
 *
 *  Created on: Jul 18, 2018
 *      Author: kevinweiss
 */
#include "port.h"
#include "stm32f1xx_hal.h"
#ifdef BLUEPILL
#include "usb_device.h"
#endif

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern void _Error_Handler(char *, int);

ADC_HandleTypeDef hadc_pm;
ADC_HandleTypeDef hadc_dut;
DMA_HandleTypeDef hdma_adc_pm;

I2C_HandleTypeDef hi2c_dut;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi_dut;

TIM_HandleTypeDef htim_ic;
TIM_HandleTypeDef htim_pwm;

UART_HandleTypeDef huart_if;
UART_HandleTypeDef huart_dut;
DMA_HandleTypeDef hdma_usart_if_rx;
DMA_HandleTypeDef hdma_usart_if_tx;
DMA_HandleTypeDef hdma_usart_dut_rx;
DMA_HandleTypeDef hdma_usart_dut_tx;

#ifdef BLUEPILL
/* ADC1 init function */
static void MX_ADC1_Init(void) {

	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	 */
	hadc_pm.Instance = ADC1;
	hadc_pm.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc_pm.Init.ContinuousConvMode = ENABLE;
	hadc_pm.Init.DiscontinuousConvMode = DISABLE;
	hadc_pm.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc_pm.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc_pm.Init.NbrOfConversion = 3;
	if (HAL_ADC_Init(&hadc_pm) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_9;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc_pm, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc_pm, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc_pm, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}


}

/* ADC2 init function */
static void MX_ADC2_Init(void) {

	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	 */
	hadc_dut.Instance = ADC2;
	hadc_dut.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc_dut.Init.ContinuousConvMode = ENABLE;
	hadc_dut.Init.DiscontinuousConvMode = DISABLE;
	hadc_dut.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc_dut.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc_dut.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc_dut, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* I2C1 init function */
static void MX_I2C1_Init(void) {

	hi2c_dut.Instance = I2C1;
	hi2c_dut.Init.ClockSpeed = 400000;
	hi2c_dut.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c_dut.Init.OwnAddress1 = 170;
	hi2c_dut.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c_dut.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE;
	hi2c_dut.Init.OwnAddress2 = 128;
	hi2c_dut.Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
	hi2c_dut.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* SPI1 init function */
static void MX_SPI1_Init(void) {

	/* SPI1 parameter configuration*/
	hspi_dut.Instance = SPI1;
	hspi_dut.Init.Mode = SPI_MODE_SLAVE;
	hspi_dut.Init.Direction = SPI_DIRECTION_2LINES;
	hspi_dut.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi_dut.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi_dut.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi_dut.Init.NSS = SPI_NSS_HARD_INPUT;
	hspi_dut.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi_dut.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi_dut.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi_dut.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi_dut.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM1 init function */
static void MX_TIM1_Init(void) {

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

	htim_ic.Instance = TIM1;
	htim_ic.Init.Prescaler = 0;
	htim_ic.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_ic.Init.Period = 0;
	htim_ic.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_ic.Init.RepetitionCounter = 0;
	htim_ic.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_IC_Init(&htim_ic) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim_ic, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim_ic, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM4 init function */
static void MX_TIM4_Init(void) {

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim_pwm.Instance = TIM4;
	htim_pwm.Init.Prescaler = 0;
	htim_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_pwm.Init.Period = 0;
	htim_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_pwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim_pwm) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim_pwm, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_3)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.Pulse = 32768;
	if (HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_4)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_TIM_MspPostInit(&htim_pwm);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void) {

	huart_if.Instance = USART1;
	huart_if.Init.BaudRate = 115200;
	huart_if.Init.WordLength = UART_WORDLENGTH_8B;
	huart_if.Init.StopBits = UART_STOPBITS_1;
	huart_if.Init.Parity = UART_PARITY_NONE;
	huart_if.Init.Mode = UART_MODE_TX_RX;
	huart_if.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart_if.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart_if) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART3 init function */
static void MX_USART3_UART_Init(void) {

	huart_dut.Instance = USART3;
	huart_dut.Init.BaudRate = 115200;
	huart_dut.Init.WordLength = UART_WORDLENGTH_8B;
	huart_dut.Init.StopBits = UART_STOPBITS_1;
	huart_dut.Init.Parity = UART_PARITY_NONE;
	huart_dut.Init.Mode = UART_MODE_TX_RX;
	huart_dut.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart_dut.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* DMA1_Channel2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	/* DMA1_Channel3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
	/* DMA1_Channel4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, DEBUG0_Pin|DEBUG1_Pin|DEBUG2_Pin|TEST_PASS_Pin
					  |TEST_WARN_Pin|TEST_FAIL_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DUT_RTS_GPIO_Port, DUT_RTS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : LED0_Pin */
	GPIO_InitStruct.Pin = LED0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DEBUG0_Pin DEBUG1_Pin DEBUG2_Pin TEST_PASS_Pin
	TEST_WARN_Pin TEST_FAIL_Pin */
	GPIO_InitStruct.Pin = DEBUG0_Pin|DEBUG1_Pin|DEBUG2_Pin|TEST_PASS_Pin
						  |TEST_WARN_Pin|TEST_FAIL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : DUT_RST_Pin */
	GPIO_InitStruct.Pin = DUT_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DUT_RST_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DUT_CTS_Pin */
	GPIO_InitStruct.Pin = DUT_CTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(DUT_CTS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DUT_RTS_Pin */
	GPIO_InitStruct.Pin = DUT_RTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DUT_RTS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USER_BTN_Pin */
	GPIO_InitStruct.Pin = USER_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC
			| RCC_PERIPHCLK_USB;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
#endif

#ifdef NUCLEOF103RB
/* ADC1 init function */
static void MX_ADC1_Init(void) {

	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	 */
	hadc_pm.Instance = ADC1;
	hadc_pm.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc_pm.Init.ContinuousConvMode = ENABLE;
	hadc_pm.Init.DiscontinuousConvMode = DISABLE;
	hadc_pm.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc_pm.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc_pm.Init.NbrOfConversion = 3;
	if (HAL_ADC_Init(&hadc_pm) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_10;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc_pm, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_11;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc_pm, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc_pm, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* ADC2 init function */
static void MX_ADC2_Init(void) {

	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	 */
	hadc_dut.Instance = ADC2;
	hadc_dut.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc_dut.Init.ContinuousConvMode = ENABLE;
	hadc_dut.Init.DiscontinuousConvMode = DISABLE;
	hadc_dut.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc_dut.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc_dut.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc_dut, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* I2C1 init function */
static void MX_I2C1_Init(void) {

	hi2c_dut.Instance = I2C1;
	hi2c_dut.Init.ClockSpeed = 400000;
	hi2c_dut.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c_dut.Init.OwnAddress1 = 170;
	hi2c_dut.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c_dut.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE;
	hi2c_dut.Init.OwnAddress2 = 128;
	hi2c_dut.Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
	hi2c_dut.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* SPI2 init function */
static void MX_SPI2_Init(void) {

	/* SPI2 parameter configuration*/
	hspi_dut.Instance = SPI2;
	hspi_dut.Init.Mode = SPI_MODE_SLAVE;
	hspi_dut.Init.Direction = SPI_DIRECTION_2LINES;
	hspi_dut.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi_dut.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi_dut.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi_dut.Init.NSS = SPI_NSS_HARD_INPUT;
	hspi_dut.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi_dut.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi_dut.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi_dut.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi_dut.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM1 init function */
static void MX_TIM1_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

	htim_ic.Instance = TIM1;
	htim_ic.Init.Prescaler = 0;
	htim_ic.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_ic.Init.Period = 0;
	htim_ic.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_ic.Init.RepetitionCounter = 0;
	htim_ic.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim_ic) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim_ic, &sClockSourceConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_IC_Init(&htim_ic) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim_ic, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim_ic, &sConfigIC, TIM_CHANNEL_1)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM3 init function */
static void MX_TIM3_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim_pwm.Instance = TIM3;
	htim_pwm.Init.Prescaler = 0;
	htim_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_pwm.Init.Period = 0;
	htim_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_pwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim_pwm) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim_pwm, &sClockSourceConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_PWM_Init(&htim_pwm) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim_pwm, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_3)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.Pulse = 32768;
	if (HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_4)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_TIM_MspPostInit(&htim_pwm);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void) {

	huart_dut.Instance = USART1;
	huart_dut.Init.BaudRate = 115200;
	huart_dut.Init.WordLength = UART_WORDLENGTH_8B;
	huart_dut.Init.StopBits = UART_STOPBITS_1;
	huart_dut.Init.Parity = UART_PARITY_NONE;
	huart_dut.Init.Mode = UART_MODE_TX_RX;
	huart_dut.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart_dut.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart_dut) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART2 init function */
static void MX_USART2_UART_Init(void) {

	huart_if.Instance = USART2;
	huart_if.Init.BaudRate = 115200;
	huart_if.Init.WordLength = UART_WORDLENGTH_8B;
	huart_if.Init.StopBits = UART_STOPBITS_1;
	huart_if.Init.Parity = UART_PARITY_NONE;
	huart_if.Init.Mode = UART_MODE_TX_RX;
	huart_if.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart_if.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart_if) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE()
	;

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* DMA1_Channel4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	/* DMA1_Channel7_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, TEST_WARN_Pin|TEST_FAIL_Pin|TEST_PASS_Pin|LED0_Pin
						  |DUT_RTS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, DEBUG0_Pin|DEBUG1_Pin|DEBUG2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : USER_BTN_Pin */
	GPIO_InitStruct.Pin = USER_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : TEST_WARN_Pin TEST_FAIL_Pin TEST_PASS_Pin LED0_Pin
						   DUT_RTS_Pin */
	GPIO_InitStruct.Pin = TEST_WARN_Pin|TEST_FAIL_Pin|TEST_PASS_Pin|LED0_Pin
						  |DUT_RTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : DUT_RST_Pin DEBUG0_Pin DEBUG1_Pin DEBUG2_Pin */
	GPIO_InitStruct.Pin = DUT_RST_Pin|DEBUG0_Pin|DEBUG1_Pin|DEBUG2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : DUT_CTS_Pin */
	GPIO_InitStruct.Pin = DUT_CTS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(DUT_CTS_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
#endif

/* IWDG init function */
static void MX_IWDG_Init(void) {

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
	hiwdg.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* RTC init function */
static void MX_RTC_Init(void) {

	/* USER CODE BEGIN RTC_Init 0 */

	/* USER CODE END RTC_Init 0 */

	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef DateToUpdate;

	/* USER CODE BEGIN RTC_Init 1 */

	/* USER CODE END RTC_Init 1 */

	/**Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	/* USER CODE BEGIN RTC_Init 2 */

	/* USER CODE END RTC_Init 2 */

	/**Initialize RTC and set the Time and Date
	 */
	sTime.Hours = 1;
	sTime.Minutes = 0;
	sTime.Seconds = 0;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	/* USER CODE BEGIN RTC_Init 3 */

	/* USER CODE END RTC_Init 3 */

	DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
	DateToUpdate.Month = RTC_MONTH_JANUARY;
	DateToUpdate.Date = 1;
	DateToUpdate.Year = 18;

	if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
	/* USER CODE BEGIN RTC_Init 4 */

	/* USER CODE END RTC_Init 4 */

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

void init_periphs(void) {
	/* Prevents i2c clk from toggling at init */
	__HAL_RCC_I2C1_CLK_ENABLE();

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
#ifdef BLUEPILL
	MX_USART3_UART_Init();
	MX_SPI1_Init();
	MX_TIM4_Init();
	MX_USB_DEVICE_Init();

#endif
#ifdef NUCLEOF103RB
	MX_USART2_UART_Init();
	MX_SPI2_Init();
	MX_TIM3_Init();
#endif
	MX_I2C1_Init();
	MX_TIM1_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_IWDG_Init();
	MX_RTC_Init();
}

void init_clock(void) {
	SystemClock_Config();
}


