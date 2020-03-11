/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/*
 ******************************************************************************
 * @file			port.h
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Board/cpu specific porting.
 ******************************************************************************
 */

#ifndef PORT_H_
#define PORT_H_

/******************************************************************************/
/* <TEMPLATE defines> *********************************************************/
/******************************************************************************/
/*
 * <MODULE>_INST
 *
 * <MODULE>_CLK_EN
 * <MODULE>_CLK_DIS
 * <MODULE>_GPIO_CLK_EN
 *
 * <MODULE>_GPIO_AF_REMAP
 *
 * <MODULE>_INT
 * <MODULE>_IRQ
 *
 * <MODULE>_DMA_INST
 * <MODULE>_DMA_INT
 * <MODULE>_DMA_IRQ
 */

#ifdef BLUEPILL
/******************************************************************************/
/******************************* Pin and Ports defines ************************/
/******************************************************************************/
#define LED0_Pin GPIO_PIN_13
#define LED0_GPIO_Port GPIOC

#define DEBUG0_Pin GPIO_PIN_0
#define DEBUG0_GPIO_Port GPIOA
#define DEBUG1_Pin GPIO_PIN_1
#define DEBUG1_GPIO_Port GPIOA
#define DEBUG2_Pin GPIO_PIN_2
#define DEBUG2_GPIO_Port GPIOA

/******************************************************************************/
#define TEST_PASS_Pin GPIO_PIN_3
#define TEST_PASS_GPIO_Port GPIOA
#define TEST_WARN_Pin GPIO_PIN_4
#define TEST_WARN_GPIO_Port GPIOA
#define TEST_FAIL_Pin GPIO_PIN_5
#define TEST_FAIL_GPIO_Port GPIOA

#define USER_BTN_Pin GPIO_PIN_15
#define USER_BTN_GPIO_Port GPIOB
#define DUT_RST_Pin GPIO_PIN_12
#define DUT_RST_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_ADC_Pin GPIO_PIN_6
#define DUT_ADC_GPIO_Port GPIOA

#define PM_LO_ADC_Pin GPIO_PIN_7
#define PM_LO_ADC_GPIO_Port GPIOA
#define PM_HI_ADC_Pin GPIO_PIN_0
#define PM_HI_ADC_GPIO_Port GPIOB
#define PM_V_ADC_Pin GPIO_PIN_1
#define PM_V_ADC_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_TX_Pin GPIO_PIN_10
#define DUT_TX_GPIO_Port GPIOB
#define DUT_RX_Pin GPIO_PIN_11
#define DUT_RX_GPIO_Port GPIOB
#define DUT_CTS_Pin GPIO_PIN_13
#define DUT_CTS_GPIO_Port GPIOB
#define DUT_RTS_Pin GPIO_PIN_14
#define DUT_RTS_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_IC_Pin GPIO_PIN_8
#define DUT_IC_GPIO_Port GPIOA

/******************************************************************************/
#define IF_TX_Pin GPIO_PIN_9
#define IF_TX_GPIO_Port GPIOA
#define IF_RX_Pin GPIO_PIN_10
#define IF_RX_GPIO_Port GPIOA

/******************************************************************************/
#define DUT_NSS_Pin GPIO_PIN_15
#define DUT_NSS_GPIO_Port GPIOA
#define DUT_SCK_Pin GPIO_PIN_3
#define DUT_SCK_GPIO_Port GPIOB
#define DUT_MISO_Pin GPIO_PIN_4
#define DUT_MISO_GPIO_Port GPIOB
#define DUT_MOSI_Pin GPIO_PIN_5
#define DUT_MOSI_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_SCL_Pin GPIO_PIN_6
#define DUT_SCL_GPIO_Port GPIOB
#define DUT_SDA_Pin GPIO_PIN_7
#define DUT_SDA_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_PWM_Pin GPIO_PIN_8
#define DUT_PWM_GPIO_Port GPIOB
#define DUT_DAC_Pin GPIO_PIN_9
#define DUT_DAC_GPIO_Port GPIOB

/******************************************************************************/
#define LED_OFF				GPIO_PIN_SET

/******************************************************************************/
#define GPIO_DEBUG0_IRQ		EXTI0_IRQn
#define GPIO_DEBUG1_IRQ		EXTI1_IRQn
#define GPIO_DEBUG2_IRQ		EXTI2_IRQn
#define GPIO_DEBUG0_INT		EXTI0_IRQHandler
#define GPIO_DEBUG1_INT		EXTI1_IRQHandler
#define GPIO_DEBUG2_INT		EXTI2_IRQHandler
#define GPIO_NSS_CTS_IRQ		EXTI15_10_IRQn
#define GPIO_NSS_CTS_INT		EXTI15_10_IRQHandler

/******************************************************************************/
/* SPI defines ****************************************************************/
/******************************************************************************/
#define DUT_SPI_INST			SPI1

#define DUT_SPI_CLK_EN()		__HAL_RCC_SPI1_CLK_ENABLE()
#define DUT_SPI_CLK_DIS()		__HAL_RCC_SPI1_CLK_DISABLE()
#define DUT_SPI_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_SPI_GPIO_AF_REMAP()	__HAL_AFIO_REMAP_SPI1_ENABLE()

#define DUT_SPI_INT				SPI1_IRQHandler
#define DUT_SPI_IRQ				SPI1_IRQn

/******************************************************************************/
/* I2C defines ****************************************************************/
/******************************************************************************/
#define DUT_I2C_INST			I2C1

#define DUT_I2C_CLK_EN()		__HAL_RCC_I2C1_CLK_ENABLE()
#define DUT_I2C_CLK_DIS()		__HAL_RCC_I2C1_CLK_DISABLE()
#define DUT_I2C_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_I2C_GPIO_AF_REMAP()

#define DUT_I2C_EV_INT			I2C1_EV_IRQHandler
#define DUT_I2C_EV_IRQ			I2C1_EV_IRQn
#define DUT_I2C_ERR_INT			I2C1_ER_IRQHandler
#define DUT_I2C_ERR_IRQ			I2C1_ER_IRQn

/******************************************************************************/
/* UART defines ***************************************************************/
/******************************************************************************/
#define DUT_UART_INST			USART3

#define DUT_UART_CLK_EN()		__HAL_RCC_USART3_CLK_ENABLE()
#define DUT_UART_CLK_DIS()		__HAL_RCC_USART3_CLK_DISABLE()
#define DUT_UART_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_UART_INT			USART3_IRQHandler
#define DUT_UART_IRQ			USART3_IRQn

#define DUT_UART_RX_DMA_INST	DMA1_Channel3
#define DUT_UART_DMA_RX_INT		DMA1_Channel3_IRQHandler
#define DUT_UART_DMA_RX_IRQ		DMA1_Channel3_IRQn

/******************************************************************************/
#define IF_UART_INST			USART1

#define IF_UART_CLK_EN()		__HAL_RCC_USART1_CLK_ENABLE()
#define IF_UART_CLK_DIS()		__HAL_RCC_USART1_CLK_DISABLE()
#define IF_UART_CLK_GPIO_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define IF_UART_INT				USART1_IRQHandler
#define IF_UART_IRQ				USART1_IRQn

#define IF_UART_DMA_RX_INST		DMA1_Channel5
#define IF_UART_DMA_RX_INT		DMA1_Channel5_IRQHandler
#define IF_UART_DMA_RX_IRQ		DMA1_Channel5_IRQn

#define IF_UART_DMA_TX_INST		DMA1_Channel4
#define IF_UART_DMA_TX_INT		DMA1_Channel4_IRQHandler
#define IF_UART_DMA_TX_IRQ		DMA1_Channel4_IRQn

/******************************************************************************/
/* PWM_DAC defines ************************************************************/
/******************************************************************************/
#define DUT_PWM_DAC_INST			TIM4
#define DUT_PWM_TMR					DUT_PWM_DAC_INST
#define DUT_DAC_TMR					DUT_PWM_DAC_INST

#define DUT_PWM_DAC_CLK_EN()		__HAL_RCC_TIM4_CLK_ENABLE()
#define DUT_PWM_DAC_CLK_DIS()		__HAL_RCC_TIM4_CLK_DISABLE()
#define DUT_PWM_DAC_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_PWM_DAC_GPIO_AF_REMAP()

/******************************************************************************/
/* IC defines *****************************************************************/
/******************************************************************************/
#define DUT_IC_INST				TIM1
#define DUT_IC_CHANNEL			TIM_CHANNEL_1

#define DUT_IC_CLK_EN()			__HAL_RCC_TIM1_CLK_ENABLE()
#define DUT_IC_CLK_DIS()		__HAL_RCC_TIM1_CLK_DISABLE()
#define DUT_IC_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_IC_GPIO_INT			EXTI9_5_IRQHandler
#define DUT_IC_GPIO_IRQ			EXTI9_5_IRQn

#define DUT_IC_DMA_INST			DMA1_Channel2
#define DUT_IC_DMA_ID			TIM_DMA_ID_CC1
#define DUT_IC_DMA_INT			DMA1_Channel2_IRQHandler
#define DUT_IC_DMA_IRQ			DMA1_Channel2_IRQn

/******************************************************************************/
/* ADC defines ****************************************************************/
/******************************************************************************/
#define DUT_ADC_INST			ADC2
#define ADC_CHANNEL				ADC_CHANNEL_6

#define DUT_ADC_CLK_EN()		__HAL_RCC_ADC2_CLK_ENABLE()
#define DUT_ADC_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_ADC_CLK_DIS()		__HAL_RCC_ADC2_CLK_DISABLE()

#define ADC_INT					ADC1_2_IRQHandler
#define ADC_IRQ					ADC1_2_IRQn

#endif

#ifdef NUCLEOF103RB
/******************************************************************************/
/******************************* Pin and Ports defines ************************/
/******************************************************************************/
#define LED0_Pin GPIO_PIN_5
#define LED0_GPIO_Port GPIOA

#define DEBUG0_Pin GPIO_PIN_3
#define DEBUG0_GPIO_Port GPIOB
#define DEBUG1_Pin GPIO_PIN_4
#define DEBUG1_GPIO_Port GPIOB
#define DEBUG2_Pin GPIO_PIN_2
#define DEBUG2_GPIO_Port GPIOD

/******************************************************************************/
#define TEST_PASS_Pin GPIO_PIN_4
#define TEST_PASS_GPIO_Port GPIOA
#define TEST_WARN_Pin GPIO_PIN_0
#define TEST_WARN_GPIO_Port GPIOA
#define TEST_FAIL_Pin GPIO_PIN_1
#define TEST_FAIL_GPIO_Port GPIOA

#define USER_BTN_Pin GPIO_PIN_13
#define USER_BTN_GPIO_Port GPIOC
#define DUT_RST_Pin GPIO_PIN_2
#define DUT_RST_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_ADC_Pin GPIO_PIN_0
#define DUT_ADC_GPIO_Port GPIOB

#define PM_LO_ADC_Pin GPIO_PIN_1
#define PM_LO_ADC_GPIO_Port GPIOC
#define PM_HI_ADC_Pin GPIO_PIN_2
#define PM_HI_ADC_GPIO_Port GPIOC
#define PM_V_ADC_Pin GPIO_PIN_0
#define PM_V_ADC_GPIO_Port GPIOC

/******************************************************************************/
#define DUT_TX_Pin GPIO_PIN_9
#define DUT_TX_GPIO_Port GPIOA
#define DUT_RX_Pin GPIO_PIN_10
#define DUT_RX_GPIO_Port GPIOA
#define DUT_CTS_Pin GPIO_PIN_11
#define DUT_CTS_GPIO_Port GPIOA
#define DUT_RTS_Pin GPIO_PIN_12
#define DUT_RTS_GPIO_Port GPIOA

/******************************************************************************/
#define DUT_IC_Pin GPIO_PIN_8
#define DUT_IC_GPIO_Port GPIOA

/******************************************************************************/
#define IF_TX_Pin GPIO_PIN_2
#define IF_TX_GPIO_Port GPIOA
#define IF_RX_Pin GPIO_PIN_3
#define IF_RX_GPIO_Port GPIOA

/******************************************************************************/
#define DUT_NSS_Pin GPIO_PIN_12
#define DUT_NSS_GPIO_Port GPIOB
#define DUT_SCK_Pin GPIO_PIN_13
#define DUT_SCK_GPIO_Port GPIOB
#define DUT_MISO_Pin GPIO_PIN_14
#define DUT_MISO_GPIO_Port GPIOB
#define DUT_MOSI_Pin GPIO_PIN_15
#define DUT_MOSI_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_SCL_Pin GPIO_PIN_8
#define DUT_SCL_GPIO_Port GPIOB
#define DUT_SDA_Pin GPIO_PIN_9
#define DUT_SDA_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_PWM_Pin GPIO_PIN_8
#define DUT_PWM_GPIO_Port GPIOC
#define DUT_DAC_Pin GPIO_PIN_9
#define DUT_DAC_GPIO_Port GPIOC

/******************************************************************************/
#define LED_OFF 			GPIO_PIN_RESET

/******************************************************************************/
#define GPIO_NSS_CTS_IRQ	EXTI15_10_IRQn
#define GPIO_DEBUG0_IRQ		EXTI3_IRQn
#define GPIO_DEBUG1_IRQ		EXTI4_IRQn
#define GPIO_DEBUG2_IRQ		EXTI2_IRQn
#define GPIO_NSS_CTS_INT	EXTI15_10_IRQHandler
#define GPIO_DEBUG0_INT		EXTI3_IRQHandler
#define GPIO_DEBUG1_INT		EXTI4_IRQHandler
#define GPIO_DEBUG2_INT		EXTI2_IRQHandler

/******************************************************************************/
/* SPI defines ****************************************************************/
/******************************************************************************/
#define DUT_SPI_INST			SPI2

#define DUT_SPI_CLK_EN()		__HAL_RCC_SPI2_CLK_ENABLE()
#define DUT_SPI_CLK_DIS()		__HAL_RCC_SPI2_CLK_DISABLE()
#define DUT_SPI_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_SPI_GPIO_AF_REMAP()

#define DUT_SPI_INT				SPI2_IRQHandler
#define DUT_SPI_IRQ				SPI2_IRQn

/******************************************************************************/
/* I2C defines ****************************************************************/
/******************************************************************************/
#define DUT_I2C_INST			I2C1

#define DUT_I2C_CLK_EN()		__HAL_RCC_I2C1_CLK_ENABLE()
#define DUT_I2C_CLK_DIS()		__HAL_RCC_I2C1_CLK_DISABLE()
#define DUT_I2C_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_I2C_GPIO_AF_REMAP()	__HAL_AFIO_REMAP_I2C1_ENABLE()

#define DUT_I2C_EV_INT			I2C1_EV_IRQHandler
#define DUT_I2C_EV_IRQ			I2C1_EV_IRQn
#define DUT_I2C_ERR_INT			I2C1_ER_IRQHandler
#define DUT_I2C_ERR_IRQ			I2C1_ER_IRQn

/******************************************************************************/
/* UART defines ***************************************************************/
/******************************************************************************/
#define DUT_UART_INST			USART1

#define DUT_UART_CLK_EN()		__HAL_RCC_USART1_CLK_ENABLE()
#define DUT_UART_CLK_DIS()		__HAL_RCC_USART1_CLK_DISABLE()
#define DUT_UART_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_UART_INT			USART1_IRQHandler
#define DUT_UART_IRQ			USART1_IRQn

#define DUT_UART_RX_DMA_INST	DMA1_Channel5
#define DUT_UART_DMA_RX_INT		DMA1_Channel5_IRQHandler
#define DUT_UART_DMA_RX_IRQ		DMA1_Channel5_IRQn

/******************************************************************************/
#define IF_UART_INST			USART2

#define IF_UART_CLK_EN()		__HAL_RCC_USART2_CLK_ENABLE()
#define IF_UART_CLK_DIS()		__HAL_RCC_USART2_CLK_DISABLE()
#define IF_UART_CLK_GPIO_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define IF_UART_INT				USART2_IRQHandler
#define IF_UART_IRQ				USART2_IRQn

#define IF_UART_DMA_RX_INST		DMA1_Channel6
#define IF_UART_DMA_RX_INT		DMA1_Channel6_IRQHandler
#define IF_UART_DMA_RX_IRQ		DMA1_Channel6_IRQn

#define IF_UART_DMA_TX_INST		DMA1_Channel7
#define IF_UART_DMA_TX_INT		DMA1_Channel7_IRQHandler
#define IF_UART_DMA_TX_IRQ		DMA1_Channel7_IRQn

/******************************************************************************/
/* PWM_DAC defines ************************************************************/
/******************************************************************************/
#define DUT_PWM_DAC_INST			TIM3
#define DUT_PWM_TMR					DUT_PWM_DAC_INST
#define DUT_DAC_TMR					DUT_PWM_DAC_INST

#define DUT_PWM_DAC_CLK_EN()		__HAL_RCC_TIM3_CLK_ENABLE()
#define DUT_PWM_DAC_CLK_DIS()		__HAL_RCC_TIM3_CLK_DISABLE()
#define DUT_PWM_DAC_GPIO_CLK_EN()	__HAL_RCC_GPIOC_CLK_ENABLE()

#define DUT_PWM_DAC_GPIO_AF_REMAP()	__HAL_AFIO_REMAP_TIM3_ENABLE();

/******************************************************************************/
/************************************ IC defines ******************************/
/******************************************************************************/
#define DUT_IC_INST				TIM1
#define DUT_IC_CHANNEL			TIM_CHANNEL_1

#define DUT_IC_CLK_EN()			__HAL_RCC_TIM1_CLK_ENABLE()
#define DUT_IC_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_IC_CLK_DIS()		__HAL_RCC_TIM1_CLK_DISABLE()

#define DUT_IC_DMA_INST			DMA1_Channel2
#define DUT_IC_DMA_ID			TIM_DMA_ID_CC1

#define DUT_IC_DMA_INT			DMA1_Channel2_IRQHandler
#define DUT_IC_DMA_IRQ			DMA1_Channel2_IRQn
#define DUT_IC_GPIO_INT			EXTI9_5_IRQHandler
#define DUT_IC_GPIO_IRQ			EXTI9_5_IRQn

/******************************************************************************/
/************************************ ADC defines *****************************/
/******************************************************************************/
#define DUT_ADC_INST		ADC2
#define ADC_CHANNEL			ADC_CHANNEL_8

#define DUT_ADC_CLK_EN()		__HAL_RCC_ADC2_CLK_ENABLE()
#define DUT_ADC_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_ADC_CLK_DIS()		__HAL_RCC_ADC2_CLK_DISABLE()

#define ADC_INT				ADC1_2_IRQHandler
#define ADC_IRQ				ADC1_2_IRQn

#endif

/******************************************************************************/
/* Pin and ports defines ******************************************************/
/******************************************************************************/
#define LED0 LED0_GPIO_Port, LED0_Pin

#define DEBUG0 DEBUG0_GPIO_Port, DEBUG0_Pin
#define DEBUG1 DEBUG1_GPIO_Port, DEBUG1_Pin
#define DEBUG2 DEBUG2_GPIO_Port, DEBUG2_Pin

#define TEST_PASS TEST_PASS_GPIO_Port, TEST_PASS_Pin
#define TEST_WARN TEST_WARN_GPIO_Port, TEST_WARN_Pin
#define TEST_FAIL TEST_FAIL_GPIO_Port, TEST_FAIL_Pin

#define USER_BTN USER_BTN_GPIO_Port, USER_BTN_Pin
#define DUT_RST DUT_RST_GPIO_Port, DUT_RST_Pin

#define DUT_ADC DUT_ADC_GPIO_Port, DUT_ADC_Pin

#define PM_LO_ADC PM_LO_ADC_GPIO_Port, PM_LO_ADC_Pin
#define PM_HI_ADC PM_HI_ADC_GPIO_Port, PM_HI_ADC_Pin
#define PM_V_ADC PM_V_ADC_GPIO_Port, PM_V_ADC_Pin

#define DUT_TX DUT_TX_GPIO_Port, DUT_TX_Pin
#define DUT_RX DUT_RX_GPIO_Port, DUT_RX_Pin
#define DUT_CTS DUT_CTS_GPIO_Port, DUT_CTS_Pin
#define DUT_RTS DUT_RTS_GPIO_Port, DUT_RTS_Pin

#define DUT_IC DUT_IC_GPIO_Port, DUT_IC_Pin

#define IF_TX IF_TX_GPIO_Port, IF_TX_Pin
#define IF_RX IF_RX_GPIO_Port, IF_RX_Pin

#define DUT_NSS DUT_NSS_GPIO_Port, DUT_NSS_Pin
#define DUT_SCK DUT_SCK_GPIO_Port, DUT_SCK_Pin
#define DUT_MISO DUT_MISO_GPIO_Port, DUT_MISO_Pin
#define DUT_MOSI DUT_MOSI_GPIO_Port, DUT_MOSI_Pin

#define DUT_SCL DUT_SCL_GPIO_Port, DUT_SCL_Pin
#define DUT_SDA DUT_SDA_GPIO_Port, DUT_SDA_Pin

#define DUT_PWM DUT_PWM_GPIO_Port, DUT_PWM_Pin
#define DUT_DAC DUT_DAC_GPIO_Port, DUT_DAC_Pin

/******************************************************************************/
/* CPU Macros *****************************************************************/
/******************************************************************************/
#define DUT_IC_REMAINING_BUF(x)	(x.Instance->CNDTR)
#define DUT_IC_OV_OCCURED(x)	(x->Instance->SR & TIM_SR_UIF)
#define DUT_IC_OV_CLEAR(x)		x->Instance->SR &= ~(TIM_SR_UIF)

#define ADC_DATA_READY(x)	(x.Instance->SR & ADC_FLAG_EOC)

#define DEFAULT_INT_PRIO	1

/******************************************************************************/
void init_periphs(void);
void init_clock(void);
void _Error_Handler(char *file, int line);

#endif /* PORT_H_ */
